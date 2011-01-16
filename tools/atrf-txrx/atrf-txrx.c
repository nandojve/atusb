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

#include "at86rf230.h"
#include "atrf.h"
#include "misctxrx.h"


/*
 * According to IEEE 802.15.4-2003 section E.2.6, channel 15 is the only
 * channel that falls into the 802.11 guard bands in North America an Europe.
 */

#define	DEFAULT_CHANNEL	15	/* channel 15, 2425 MHz */

/*
 * Transmit power, dBm. IEEE 802.15.4-2003 section E.3.1.3 specifies a transmit
 * power of 0 dBm for IEEE 802.15.4. We assume an antenna gain of 3 dB or
 * better.
 */

#define	DEFAULT_POWER	-3.2	/* transmit power, dBm */


static double tx_pwr_230[] = {
	 3.0,	 2.6,	 2.1,	 1.6,
	 1.1,	 0.5,	-0.2,	-1.2,
	-2.2,	-3.2,	-4.2,	-5.2,
	-7.2,	-9.2,	-12.2,	-17.2
};


static double tx_pwr_231[] = {
	 3.0,	 2.8,	 2.3,	 1.8,
	 1.3,	 0.7,	 0.0,	-1,
	-2,	-3,	-4,	-5,
	-7,	-9,	-12,	-17
};


static volatile int run = 1;


/*
 * clkm: 0   disable CLKM
 *       >0  output 2^(clkm-1) MHz signal
 */

static struct atrf_dsc *init_txrx(int trim, unsigned clkm)
{
	struct atrf_dsc *dsc;

	dsc = atrf_open();
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

	if (!clkm)
		atrf_reg_write(dsc, REG_TRX_CTRL_0, 0); /* disable CLKM */
	else
		atrf_reg_write(dsc, REG_TRX_CTRL_0,
		    (PAD_IO_8mA << PAD_IO_CLKM_SHIFT) | clkm);

	/* We want to see all interrupts, not only the ones we're expecting. */
	atrf_reg_write(dsc, REG_IRQ_MASK, 0xff);

	(void) atrf_reg_read(dsc, REG_IRQ_STATUS);
	if (atrf_identify(dsc) == artf_at86rf231)
		wait_for_interrupt(dsc, IRQ_CCA_ED_DONE, IRQ_CCA_ED_DONE,
		    10, 50); /* according to table 7-1, 37 us max */

	return dsc;
}


static void set_channel(struct atrf_dsc *dsc, int channel)
{
	atrf_reg_write(dsc, REG_PHY_CC_CCA, (1 << CCA_MODE_SHIFT) | channel);
}


static void set_power(struct atrf_dsc *dsc, double power, int crc)
{
	const double *tx_pwr;
	int n;
	uint8_t tmp;

	switch (atrf_identify(dsc)) {
	case artf_at86rf230:
		tx_pwr = tx_pwr_230;
		break;
	case artf_at86rf231:
		tx_pwr = tx_pwr_231;
		break;
	default:
		abort();
	}
	
	for (n = 0; n != sizeof(tx_pwr_230)/sizeof(*tx_pwr_230)-1; n++)
		if (tx_pwr[n] <= power)
			break;
	if (fabs(tx_pwr[n]-power) > 0.01)
		fprintf(stderr, "TX power %.1f dBm\n", tx_pwr[n]);

	switch (atrf_identify(dsc)) {
	case artf_at86rf230:
		atrf_reg_write(dsc, REG_PHY_TX_PWR,
		    (crc ? TX_AUTO_CRC_ON_230 : 0) | n);
		break;
	case artf_at86rf231:
		tmp = atrf_reg_read(dsc, REG_PHY_TX_PWR);
		tmp = (tmp & ~TX_PWR_MASK) | n;
		atrf_reg_write(dsc, REG_PHY_TX_PWR, tmp);
		atrf_reg_write(dsc, REG_TRX_CTRL_1,
		    crc ? TX_AUTO_CRC_ON : 0);
		break;
	default:
		abort();
	}
}


static void receive(struct atrf_dsc *dsc)
{
	uint8_t buf[MAX_PSDU+1]; /* PSDU+LQI */
	int n, ok, i;
	uint8_t ed, lqi;

	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_RX_ON);
	/*
	 * 180 us, according to AVR2001 section 4.2. We time out after
	 * nominally 200 us.
	 */
	wait_for_interrupt(dsc, IRQ_PLL_LOCK, IRQ_PLL_LOCK, 10, 20);

	fprintf(stderr, "Ready.\n");
	wait_for_interrupt(dsc, IRQ_TRX_END, IRQ_TRX_END | IRQ_RX_START,
	    10, 0);
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


static void enter_test_mode_230(struct atrf_dsc *dsc, uint8_t cont_tx)
{
	atrf_buf_write(dsc, "", 1);
	atrf_reg_write(dsc, REG_CONT_TX_0, CONT_TX_MAGIC);
	atrf_reg_write(dsc, REG_CONT_TX_1, cont_tx);

	if (!atrf_test_mode(dsc)) {
		atrf_reset_rf(dsc);
		fprintf(stderr, "device does not support test mode\n");
		exit(1);
	}

	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_PLL_ON);
	wait_for_interrupt(dsc, IRQ_PLL_LOCK, IRQ_PLL_LOCK, 10, 20);

	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_TX_START);
}


static void enter_test_mode_231(struct atrf_dsc *dsc, uint8_t cont_tx)
{
	uint8_t buf[127];
	uint8_t status;

	switch (cont_tx) {
	case CONT_TX_M2M:
		fprintf(stderr,
		    "-2 MHz mode is not supported by the AT86RF231\n");
		atrf_close(dsc);
		exit(1);
	case CONT_TX_M500K:
		memset(buf, 0, sizeof(buf));
		break;
	case CONT_TX_P500K:
		memset(buf, 0xff, sizeof(buf));
		break;
	default:
		abort();
	}

	atrf_reg_write(dsc, REG_IRQ_MASK, IRQ_PLL_LOCK);		/* 2 */
	atrf_reg_write(dsc, REG_TRX_CTRL_1, 0);				/* 3 */
	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_FORCE_TRX_OFF);	/* 4 */
	/* deleted step 5 - we don't need to enable CLKM */

	status = atrf_reg_read(dsc, REG_TRX_STATUS) & TRX_STATUS_MASK;	/* 8 */
	if (status != TRX_STATUS_TRX_OFF) {
		fprintf(stderr, "expected status 0x%02x, got 0x%02x\n",
		    TRX_STATUS_TRX_OFF, status);
		exit(1);
	}

	atrf_reg_write(dsc, REG_CONT_TX_0, CONT_TX_MAGIC);		/* 9 */
	atrf_reg_write(dsc, REG_TRX_CTRL_2, OQPSK_DATA_RATE_2000);	/*10 */
	atrf_reg_write(dsc, REG_RX_CTRL, 0xa7);				/*11 */

	atrf_buf_write(dsc, buf, sizeof(buf));				/*12 */

	atrf_reg_write(dsc, REG_PART_NUM, 0x54);			/*13 */
	atrf_reg_write(dsc, REG_PART_NUM, 0x46);			/*14 */
	
	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_PLL_ON);		/*15 */
	wait_for_interrupt(dsc, IRQ_PLL_LOCK, IRQ_PLL_LOCK, 10, 0);	/*16 */

	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_TX_START);		/*17 */
}



static int test_mode(struct atrf_dsc *dsc, uint8_t cont_tx, const char *cmd)
{
	int status = 0;

	switch (atrf_identify(dsc)) {
	case artf_at86rf230:
		enter_test_mode_230(dsc, cont_tx);
		break;
	case artf_at86rf231:
		enter_test_mode_231(dsc, cont_tx);
		break;
	default:
		abort();
	}

	if (cmd)
		status = system(cmd);
	else {
		while (run)
			sleep(1);
	}

	if (atrf_identify(dsc) == artf_at86rf231)
		atrf_reg_write(dsc, REG_PART_NUM, 0);
	
	atrf_reset_rf(dsc);

	return status;
}


static void die(int sig)
{
	run = 0;
}


static void usage(const char *name)
{
	fprintf(stderr,
"usage: %s [-c channel|-f freq] [-p power] [-t trim] [message [repetitions]]\n"
"       %s [-c channel|-f freq] [-p power] [-t trim] -T offset [command]\n\n"
"    message     message string to send (if absent, receive)\n"
"    repetitions number of times the message is sent (default 1)\n"
"    command     shell command to run while transmitting (default: wait for\n"
"                SIGINT instead)\n\n"
"    -c channel  channel number, 11 to 26 (default %d)\n"
"    -C mhz      output clock at 1, 2, 4, 8, or 16 MHz (default: off)\n"
"    -f freq     frequency in MHz, 2405 to 2480 (default %d)\n"
"    -p power    transmit power, -17.2 to 3.0 dBm (default %.1f)\n"
"    -t trim     trim capacitor, 0 to 15 (default 0)\n"
"    -t trim     trim capacitor, 0 to 15 (default 0)\n"
"    -T offset   test mode. offset is the frequency offset of the constant\n"
"                wave in MHz: -2, -0.5, or +0.5\n"
	    , name, name, DEFAULT_CHANNEL, 2405+5*(DEFAULT_CHANNEL-11),
	    DEFAULT_POWER);
	exit(1);
}


int main(int argc, char *const *argv)
{
	int channel = DEFAULT_CHANNEL;
	double power = DEFAULT_POWER;
	int trim = 0, times = 1;
	uint8_t cont_tx = 0;
	char *end;
	int c, freq;
	unsigned tmp, clkm = 0;
	int status = 0;
	struct atrf_dsc *dsc;

	while ((c = getopt(argc, argv, "c:C:f:p:t:T:")) != EOF)
		switch (c) {
		case 'c':
			channel = strtoul(optarg, &end, 0);
			if (*end)
				usage(*argv);
			if (channel < 11 || channel > 26)
				usage(*argv);
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
		case 'p':
			power = strtod(optarg, &end);
			if (*end)
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
			tmp = strtol(optarg, &end, 0);
			if (*end)
				usage(*argv);
			if (!tmp)
				usage(*argv);
			for (clkm = 1; !(tmp & 1); tmp >>= 1)
				clkm++;
			if (tmp != 1 || clkm > 5)
				usage(*argv);
			break;
		case 'T':
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
		dsc = init_txrx(trim, clkm);
		set_channel(dsc, channel);
		if (!cont_tx)
			receive(dsc);
		else {
			set_power(dsc, power, 0);
			status = test_mode(dsc, cont_tx, NULL);
		}
		break;
	case 2:
		if (cont_tx)
			usage(*argv);
		times = strtoul(argv[optind+1], &end, 0);
		if (*end)
			usage(*argv);
		/* fall through */
	case 1:
		dsc = init_txrx(trim, clkm);
		set_channel(dsc, channel);
		if (!cont_tx) {
			set_power(dsc, power, 1);
			transmit(dsc, argv[optind], times);
		} else {
			set_power(dsc, power, 0);
			status = test_mode(dsc, cont_tx, argv[optind]);
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
