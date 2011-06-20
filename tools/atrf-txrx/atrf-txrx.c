/*
 * atrf-txrx/atrf-txrx.c - ben-wpan AT86RF230 TX/RX
 *
 * Written 2010-2011 by Werner Almesberger
 * Copyright 2010-2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/time.h>

#include "at86rf230.h"
#include "atrf.h"
#include "misctxrx.h"
#include "cwtest.h"

#include "pcap.h"


/*
 * According to IEEE 802.15.4-2003 section E.2.6, channel 15 is the only
 * channel that falls into the 802.11 guard bands in North America an Europe.
 */

#define	DEFAULT_CHANNEL	15	/* channel 15, 2425 MHz */

#define	DEFAULT_TRIM	8	/* trim range is 0-15, see also ECN0002 */


/*
 * Transmit power, dBm. IEEE 802.15.4-2003 section E.3.1.3 specifies a transmit
 * power of 0 dBm for IEEE 802.15.4. We assume an antenna gain of 3 dB or
 * better.
 */

#define	DEFAULT_POWER	-3.2	/* transmit power, dBm */


struct ping {
	uint32_t	seq;	/* sequence number from originator, > 0 */
	uint32_t	ack;	/* last sequence number received, 0 if none */
	uint8_t		pad[117]; /* pad to 127 bytes */
	uint16_t	crc;
} __attribute__((__packed__));

enum rx_res {
	rx_exit,
	rx_good,
	rx_bad,
	rx_timeout,
};


static volatile int run = 1;


/*
 * clkm: 0   disable CLKM
 *       >0  output 2^(clkm-1) MHz signal
 */

static struct atrf_dsc *init_txrx(const char *driver, int trim, unsigned mhz)
{
	struct atrf_dsc *dsc;

	dsc = atrf_open(driver);
	if (!dsc)
		exit(1);
	
	atrf_reset_rf(dsc);
	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_TRX_OFF);

#if 1 // def HAVE_USB /* @@@ yeah, ugly */
	atrf_reg_write(dsc, REG_XOSC_CTRL,
	    (XTAL_MODE_INT << XTAL_MODE_SHIFT) | trim);
#else
	atrf_reg_write(dsc, REG_XOSC_CTRL, XTAL_MODE_EXT << XTAL_MODE_SHIFT);
#endif

	if (!atrf_set_clkm(dsc, mhz))
		if (mhz) {
			atrf_close(dsc);
			exit(1);
		}

	/* We want to see all interrupts, not only the ones we're expecting. */
	atrf_reg_write(dsc, REG_IRQ_MASK, 0xff);

	flush_interrupts(dsc);
	if (atrf_identify(dsc) == artf_at86rf231)
		wait_for_interrupt(dsc, IRQ_CCA_ED_DONE, IRQ_CCA_ED_DONE,
		    10, 50); /* according to table 7-1, 37 us max */

	return dsc;
}


static void set_channel(struct atrf_dsc *dsc, int channel)
{
	atrf_reg_write(dsc, REG_PHY_CC_CCA, (1 << CCA_MODE_SHIFT) | channel);
}


static void set_rate(struct atrf_dsc *dsc, uint8_t rate)
{
	if (!rate)
		return;
	switch (atrf_identify(dsc)) {
	case artf_at86rf230:
		fprintf(stderr, "AT86RF230 only supports 250 kbps\n");
		break;
	case artf_at86rf231:
		atrf_reg_write(dsc, REG_TRX_CTRL_2, rate);
		break;
	default:
		abort();
	}
}


static void receive_message(struct atrf_dsc *dsc)
{
	uint8_t buf[MAX_PSDU+1]; /* PSDU+LQI */
	int n, ok, i;
	uint8_t ed, lqi;

	fprintf(stderr, "Ready.\n");
	wait_for_interrupt(dsc, IRQ_TRX_END,
	    IRQ_TRX_END | IRQ_RX_START | IRQ_AMI, 10, 0);
	if (!run)
		return;

	n = atrf_buf_read(dsc, buf, sizeof(buf));
	if (n < 0)
		exit(1);
	if (n < 3) {
		fprintf(stderr, "%d bytes received\n", n);
		exit(1);
	}
	ed = atrf_reg_read(dsc, REG_PHY_ED_LEVEL);
	ok = !!(atrf_reg_read(dsc, REG_PHY_RSSI) & RX_CRC_VALID);
	lqi = buf[n-1];
	fprintf(stderr, "%d bytes payload, CRC %s, LQI %u, ED %d dBm\n",
	    n-3, ok ? "OK" : "BAD", lqi, -91+ed);
	for (i = 0; i != n-3; i++)
		putchar(buf[i] < ' ' || buf[i] > '~' ? '?' : buf[i]);
	putchar('\n');
}


static void write_pcap_hdr(FILE *file)
{
	struct pcap_file_header hdr = {
		.magic		= PCAP_FILE_MAGIC,
		.version_major	= 2,
		.version_minor	= 4,
		.thiszone	= 0,
		.sigfigs	= 0,
		.snaplen	= MAX_PSDU,
		.linktype	= DLT_IEEE802_15_4
	};

	if (fwrite(&hdr, sizeof(hdr), 1, file) != 1) {
		perror("fwrite");
		exit(1);
	}
}


static void write_pcap_rec(FILE *file, const struct timeval *tv,
    const void *buf, int n)
{
	struct pcap_pkthdr hdr = {
		.ts_sec		= tv->tv_sec,
		.ts_usec	= tv->tv_usec,
		.caplen		= n,
		.len		= n
	};

	if (fwrite(&hdr, sizeof(hdr), 1, file) != 1) {
		perror("fwrite");
		exit(1);
	}
	if (fwrite(buf, n, 1, file) != 1) {
		perror("fwrite");
		exit(1);
	}
}


static void receive_pcap(struct atrf_dsc *dsc, const char *name)
{
	FILE *file;
	uint8_t buf[MAX_PSDU+1]; /* PSDU+LQI */
	struct timeval now;
	int n;
	int count = 0;

	file = fopen(name, "w");
	if (!file) {
		perror(name);
		exit(1);
	}
	write_pcap_hdr(file);
	while (run) {
		wait_for_interrupt(dsc,
		    IRQ_TRX_END, IRQ_TRX_END | IRQ_RX_START | IRQ_AMI,
		    10, 0);
		if (!run)
			break;
		gettimeofday(&now, NULL);
		n = atrf_buf_read(dsc, buf, sizeof(buf));
		if (n < 0)
			exit(1);
		if (n < 2) {
			fprintf(stderr, "%d bytes received\n", n);
			continue;
		}
		write_pcap_rec(file, &now, buf, n-1);
		(void) write(2, ".", 1);
		count++;
	}
	if (fclose(file) == EOF) {
		perror(name);
		exit(1);
	}
	fprintf(stderr, "%sreceived %d message%s\n", count ? "\n" : "",
	    count, count == 1 ? "" : "s");
}


static void receive(struct atrf_dsc *dsc, const char *name)
{
	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_RX_ON);
	/*
	 * 180 us, according to AVR2001 section 4.2. We time out after
	 * nominally 200 us.
	 */
	wait_for_interrupt(dsc, IRQ_PLL_LOCK, IRQ_PLL_LOCK, 10, 20);

	if (name)
		receive_pcap(dsc, name);
	else
		receive_message(dsc);
}


static void transmit(struct atrf_dsc *dsc, const char *msg, int times)
{
	uint8_t buf[MAX_PSDU];

	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_PLL_ON);
	/*
	 * 180 us, according to AVR2001 section 4.3. We time out after
	 * nominally 200 us.
	 */
	wait_for_interrupt(dsc, IRQ_PLL_LOCK, IRQ_PLL_LOCK, 10, 20);

	/*
	 * We need to copy the message to append the CRC placeholders.
	 */
	strcpy((void *) buf, msg);
	atrf_buf_write(dsc, buf, strlen(msg)+2);

	while (run && times--) {
		/* @@@ should wait for clear channel */
		atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_TX_START);

		/* wait up to 10 ms (nominally) */
		wait_for_interrupt(dsc, IRQ_TRX_END,
		   IRQ_TRX_END | IRQ_PLL_LOCK, 10, 1000);
	}
}


static void transmit_pattern(struct atrf_dsc *dsc, double pause_s, int times)
{
	uint8_t buf[MAX_PSDU];
	uint8_t n = 0;
	int us = fmod(pause_s, 1)*1000000;

	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_PLL_ON);
	/*
	 * 180 us, according to AVR2001 section 4.3. We time out after
	 * nominally 200 us.
	 */
	wait_for_interrupt(dsc, IRQ_PLL_LOCK, IRQ_PLL_LOCK, 10, 20);

	while (run) {
		memset(buf, n, sizeof(buf));
		atrf_buf_write(dsc, buf, sizeof(buf));

		atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_TX_START);

		/* wait up to 10 ms (nominally) */
		wait_for_interrupt(dsc, IRQ_TRX_END,
		   IRQ_TRX_END | IRQ_PLL_LOCK, 10, 1000);

		if (pause_s >= 1)
			sleep(pause_s);
		if (us)
			usleep(us);

		if (times && !--times)
			break;
		n++;
	}
}


static void ping_tx(struct atrf_dsc *dsc, const struct ping *pck)
{
	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_PLL_ON);
	/*
	 * 180 us, according to AVR2001 section 4.3. We time out after
	 * nominally 200 us.
	 */
	wait_for_interrupt(dsc, IRQ_PLL_LOCK, IRQ_PLL_LOCK, 10, 20);

	atrf_buf_write(dsc, pck, sizeof(*pck));
	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_TX_START);

	/* wait up to 10 ms (nominally) */
	wait_for_interrupt(dsc, IRQ_TRX_END,
	   IRQ_TRX_END | IRQ_PLL_LOCK, 10, 1000);
}


static enum rx_res ping_rx(struct atrf_dsc *dsc, struct ping *pck, int wait_ds)
{
	uint8_t irq;
	int n;

	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_RX_ON);
	irq = wait_for_interrupt(dsc, IRQ_TRX_END,
	    IRQ_TRX_END | IRQ_RX_START | IRQ_PLL_LOCK,
	    100000, wait_ds);
	if (!run)
		return rx_exit;
	if (!irq)
		return rx_timeout;

	n = atrf_buf_read(dsc, pck, sizeof(*pck));
	if (n < 0)
		exit(1);
	if (n != sizeof(*pck)) {
		fprintf(stderr, "%d bytes received\n", n);
		return rx_bad;
	}
	return atrf_reg_read(dsc, REG_PHY_RSSI) & RX_CRC_VALID ?
	    rx_good : rx_bad;
}


static void ping(struct atrf_dsc *dsc, double max_wait_s, int master)
{
	static int first = 1;
	struct ping tx_pck = {
		.seq = 0,
		.ack = 0,
	};
	struct ping rx_pck;
	enum rx_res res;

	while (run) {
		tx_pck.seq++;
		if (master || !first) {
			ping_tx(dsc, &tx_pck);
			if (!run)
				break;
		}
		first = 0;
		res = ping_rx(dsc, &rx_pck, master ? max_wait_s*10 : 0);
		switch (res) {
		case rx_good:
			tx_pck.ack = rx_pck.seq;
			if (tx_pck.seq == rx_pck.ack)
				write(2, ".", 1);
			else
				write(2, "*", 1);
			break;
		case rx_bad:
			write(2, "-", 1);
			break;
		case rx_timeout:
			write(2, "+", 1);
			break;
		case rx_exit:
			return;
		default:
			abort();
		}
	}
}


static int test_mode(struct atrf_dsc *dsc, uint8_t cont_tx, const char *cmd)
{
	int status = 0;

	cw_test_begin(dsc, cont_tx);

	if (cmd)
		status = system(cmd);
	else {
		while (run)
			sleep(1);
	}

	cw_test_end(dsc);

	return status;
}


static void die(int sig)
{
	run = 0;
}


static void usage(const char *name)
{
	fprintf(stderr,
"usage: %s [common_options] [message [repetitions]]\n"
"       %s [common_options] -E pause_s [repetitions]\n"
"       %s [common_options] -P [max_wait_s]\n"
"       %s [common_options] -T offset [command]\n\n"
"  text message mode:\n"
"    message     message string to send (if absent, receive)\n"
"    repetitions number of times the message is sent (default 1)\n\n"
"  PER test mode (transmit only):\n"
"    -E pause_s  seconds to pause between frames (floating-point)\n"
"    repetitions number of messages to send (default: infinite)\n\n"
"  Ping-pong mode:\n"
"    -P          exchange packets between two stations\n"
"    max_wait_s  generate a new packet if no response is received (master)\n\n"
"  constant wave test mode (transmit only):\n"
"    -T offset   test mode. offset is the frequency offset of the constant\n"
"                wave in MHz: -2, -0.5, or +0.5\n"
"    command     shell command to run while transmitting (default: wait for\n"
"                SIGINT instead)\n\n"
"  common options: [-c channel|-f freq] [-C mhz] [-d driver[:arg]] [-o file]\n"
"                  [-p power] [-r rate] [-t trim]\n"
"    -c channel  channel number, 11 to 26 (default %d)\n"
"    -C mhz      output clock at 1, 2, 4, 8, or 16 MHz (default: off)\n"
"    -d driver[:arg]\n"
"                use the specified driver (default: %s)\n"
"    -f freq     frequency in MHz, 2405 to 2480 (default %d)\n"
"    -o file     write received data to a file in pcap format\n"
"    -p power    transmit power, -17.2 to 3.0 dBm (default %.1f)\n"
"    -r rate     data rate, 250k, 500k, 1M, or 2M (default: 250k)\n"
"    -t trim     trim capacitor, 0 to 15 (default %d)\n"
	    , name, name, name, name,
	    DEFAULT_CHANNEL, atrf_default_driver_name(),
	    2405+5*(DEFAULT_CHANNEL-11), DEFAULT_POWER,
	    DEFAULT_TRIM);
	exit(1);
}


int main(int argc, char *const *argv)
{
	enum {
		mode_msg,
		mode_per,
		mode_ping,
		mode_cont_tx,
	} mode = mode_msg;
	const char *driver = NULL;
	int channel = DEFAULT_CHANNEL;
	double power = DEFAULT_POWER;
	uint8_t rate = OQPSK_DATA_RATE_250;
	int trim = DEFAULT_TRIM, times = 1;
	uint8_t cont_tx = 0;
	double pause_s = 0;
	char *end;
	int c, freq;
	unsigned clkm = 0;
	int status = 0;
	const char *pcap_file = NULL;
	struct atrf_dsc *dsc;

	while ((c = getopt(argc, argv, "c:C:d:f:o:p:r:E:Pt:T:")) != EOF)
		switch (c) {
		case 'c':
			channel = strtoul(optarg, &end, 0);
			if (*end)
				usage(*argv);
			if (channel < 11 || channel > 26)
				usage(*argv);
			break;
		case 'd':
			driver = optarg;
			break;
		case 'f':
			freq = strtoul(optarg, &end, 0);
			if (*end)
				usage(*argv);
			if (freq % 5)
				usage(*argv);
			channel = (freq-2405)/5+11;
			if (channel < 11 || channel > 26)
				usage(*argv);
			break;
		case 'o':
			pcap_file = optarg;
			break;
		case 'p':
			power = strtod(optarg, &end);
			if (*end)
				usage(*argv);
			break;
		case 'r':
			if (!strcmp(optarg, "250k"))
				rate = OQPSK_DATA_RATE_250;
			else if (!strcmp(optarg, "500k"))
				rate = OQPSK_DATA_RATE_500;
			else if (!strcmp(optarg, "1M"))
				rate = OQPSK_DATA_RATE_1000;
			else if (!strcmp(optarg, "2M"))
				rate = OQPSK_DATA_RATE_2000;
			else
				usage(*argv);
			break;
		case 't':
			trim = strtoul(optarg, &end, 0);
			if (*end)
				usage(*argv);
			if (trim > 15)
				usage(*argv);
			break;
		case 'C':
			clkm = strtol(optarg, &end, 0);
			if (*end)
				usage(*argv);
			if (!clkm)
				usage(*argv);
			break;
		case 'E':
			mode = mode_per;
			pause_s = strtof(optarg, &end);
			if (*end)
				usage(*argv);
			break;
		case 'P':
			mode = mode_ping;
			break;
		case 'T':
			mode = mode_cont_tx;
			if (!strcmp(optarg, "-2"))
				cont_tx = CONT_TX_M2M;
			else if (!strcmp(optarg, "-0.5"))
				cont_tx = CONT_TX_M500K;
			else if (!strcmp(optarg, "+0.5"))
				cont_tx = CONT_TX_P500K;
			else
				usage(*argv);
			break;
		default:
			usage(*argv);
		}

	signal(SIGINT, die);

	switch (argc-optind) {
	case 0:
		dsc = init_txrx(driver, trim, clkm);
		set_channel(dsc, channel);
		set_rate(dsc, rate);
		switch (mode) {
		case mode_msg:
			receive(dsc, pcap_file);
			break;
		case mode_per:
			set_power_dBm(dsc, power, 0);
			transmit_pattern(dsc, pause_s, 0);
			break;
		case mode_ping:
			set_power_dBm(dsc, power, 1);
			ping(dsc, pause_s, 0);
			break;
		case mode_cont_tx:
			set_power_dBm(dsc, power, 0);
			status = test_mode(dsc, cont_tx, NULL);
			break;
		default:
			abort();
		}
		break;
	case 2:
		switch (mode) {
		case mode_msg:
			break;
		case mode_per:
		case mode_ping:
			/* fall through */
		case mode_cont_tx:
			usage(*argv);
		default:
			abort();
		}
		times = strtoul(argv[optind+1], &end, 0);
		if (*end)
			usage(*argv);
		/* fall through */
	case 1:
		dsc = init_txrx(driver, trim, clkm);
		set_channel(dsc, channel);
		set_rate(dsc, rate);
		switch (mode) {
		case mode_msg:
			set_power_dBm(dsc, power, 1);
			transmit(dsc, argv[optind], times);
			break;
		case mode_per:
			times = strtoul(argv[optind], &end, 0);
			if (*end)
				usage(*argv);
			set_power_dBm(dsc, power, 0);
			transmit_pattern(dsc, pause_s, times);
			break;
		case mode_ping:
			pause_s = strtof(argv[optind], &end);
			if (*end)
				usage(*argv);
			set_power_dBm(dsc, power, 1);
			ping(dsc, pause_s, 1);
			break;
		case mode_cont_tx:
			set_power_dBm(dsc, power, 0);
			status = test_mode(dsc, cont_tx, argv[optind]);
			break;
		default:
			abort();
		
		}
		break;
	default:
		usage(*argv);
	}

	atrf_close(dsc);

	if (status) {
		if (WIFEXITED(status))
			return WEXITSTATUS(status);
		if (WIFSIGNALED(status))
			raise(WTERMSIG(status));
		fprintf(stderr, "unexpected exit status %d\n", status);
		abort();
	}
	return 0;
}
