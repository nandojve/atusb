/*
 * atrf-xmit/atrf-xmit.c - Fast transmission test
 *
 * Written 2011 by Werner Almesberger
 * Copyright 2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "at86rf230.h"

#include "misctxrx.h"
#include "atrf.h"


#define	DEFAULT_CHANNEL	15
#define	DEFAULT_TRIM	8
#define	DEFAULT_POWER	15

#define	PSDU_SIZE	127


static int verbose = 0;


static void init_common(struct atrf_dsc *dsc, int trim, int channel)
{
	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_TRX_OFF);
	atrf_reg_write(dsc, REG_XOSC_CTRL,
	    (XTAL_MODE_INT << XTAL_MODE_SHIFT) | trim);
	atrf_set_clkm(dsc, 0);
	atrf_reg_write(dsc, REG_PHY_CC_CCA, (1 << CCA_MODE_SHIFT) | channel);
	atrf_reg_write(dsc, REG_IRQ_MASK, 0xff);
	flush_interrupts(dsc);
}


static void init_tx(struct atrf_dsc *dsc, int trim, int channel, int power)
{
	uint8_t buf[PSDU_SIZE];
	int i;

	init_common(dsc, trim, channel);
	set_power_step(dsc, power, 1);
	for (i = 0; i != sizeof(buf); i++)
		buf[i] = i;
	atrf_buf_write(dsc, buf, sizeof(buf));
	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_PLL_ON);
	wait_for_interrupt(dsc, IRQ_PLL_LOCK, IRQ_PLL_LOCK, 1);
}


static void init_rx(struct atrf_dsc *dsc, int trim, int channel)
{
	init_common(dsc, trim, channel);
	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_RX_ON);
	wait_for_interrupt(dsc, IRQ_PLL_LOCK, IRQ_PLL_LOCK, 1);
}


static int xfer_one(struct atrf_dsc *tx, struct atrf_dsc *rx)
{
	uint8_t irq;
	uint8_t buf[PSDU_SIZE+1]; /* +1 for LQI */
	int n, i;

	if (wait_for_interrupt(tx, IRQ_TRX_END, IRQ_TRX_END, 1)) {
		fprintf(stderr, "unexpected sender interrupt\n");
		exit(1);
	}
	if (wait_for_interrupt(rx, IRQ_TRX_END, IRQ_TRX_END, 1)) {
		fprintf(stderr, "unexpected receiver interrupt\n");
		exit(1);
	}

	atrf_slp_tr(tx, 1, 1);
	irq = wait_for_interrupt(rx, IRQ_TRX_END, IRQ_TRX_END | IRQ_RX_START,
	    10);
	if (!(irq & IRQ_TRX_END))
		return 0;

	irq = wait_for_interrupt(tx, IRQ_TRX_END, IRQ_TRX_END, 1);
	if (!(irq & IRQ_TRX_END)) {
		fprintf(stderr, "sender claims packet was not sent ?\n");
		exit(1);
	}

	if (atrf_reg_read(rx, REG_PHY_RSSI) & RX_CRC_VALID)
		return 1;
	n = atrf_buf_read(rx, buf, sizeof(buf));
	if (n <= 0)
		return 0;
	n--; /* we don't care about the LQI */
	if (n != PSDU_SIZE) {
		printf("0\n");
		return 0;
	}
	for (i = 0; i != n-2; i++)
		if (buf[i] != i)
			break;
	/*
	 * @@@ We should analyze the CRC here to see if the first or the second
	 * byte got corrupted.
	 */
	if (verbose) {
		printf("%d", i+1);
		for (i = 0; i != n-2; i++)
			printf("%s%02x", i ? " " : "\t", buf[i]);
		printf("\n");
	} else {
		printf("%d\n", i+1);
	}
	return 0;
}


static void xfer(struct atrf_dsc *tx, struct atrf_dsc *rx, int packets)
{
	int got = 0;
	int i;

	for (i = 0; i != packets; i++)
		got += xfer_one(tx, rx);
	printf("%d/%d\n", got, packets);
}


static void usage(const char *name)
{
	fprintf(stderr,
"usage: %s [-c channel] [-p power] [-t trim_tx [-t trim_rx]] [-v]\n"
"%15s driver_tx[:arg] driver_rx[:arg] [packets]\n\n"
"  -c channel  transmit/receive channel, 11 to 26 (default %d)\n"
"  -p power    transmit power, 0 to 15 (default %d)\n"
"  -t trim     trim capacitor, 0 to 15 (default %d)\n"
"  -v          verbose reporting of transmission errors\n"
    , name, "",
    DEFAULT_CHANNEL, DEFAULT_POWER, DEFAULT_TRIM);
	exit(1);
}

	
int main(int argc, char **argv)
{
	const char *tx_drv, *rx_drv;
	struct atrf_dsc *tx, *rx;
	int trim_tx = -1, trim_rx = DEFAULT_TRIM;
	int channel = DEFAULT_CHANNEL;
	int power = DEFAULT_POWER;
	int packets = 1;
	unsigned long tmp;
	char *end;
	int c;

	while ((c = getopt(argc, argv, "c:p:t:v")) != EOF)
		switch (c) {
		case 'c':
			tmp = strtoul(optarg, &end, 0);
			if (*end || tmp < 11 || tmp > 26)
				usage(*argv);
			channel = tmp;
			break;
		case 'v':
			verbose++;
			break;
		case 'p':
			tmp = strtoul(optarg, &end, 0);
			if (*end || tmp > 15)
				usage(*argv);
			power = tmp;
			break;
		case 't':
			tmp = strtoul(optarg, &end, 0);
			if (*end || tmp > 15)
				usage(*argv);
			if (trim_tx == -1)
				trim_tx = tmp;
			else
				trim_rx = tmp;
			break;
		default:
			usage(*argv);
		}

	if (trim_tx == -1)
		trim_tx = DEFAULT_TRIM;

	switch (argc-optind) {
	case 3:
		packets = strtoul(argv[optind+2], &end, 0);
		if (*end)
			usage(*argv);
		/* fall through */
	case 2:
		tx_drv = argv[optind];
		rx_drv = argv[optind+1];
		break;
	default:
		usage(*argv);
	}

	tx = atrf_open(tx_drv);
	if (!tx)
		return 1;
	rx = atrf_open(rx_drv);
	if (!rx)
		return 1;

	init_rx(rx, trim_rx, channel);
	init_tx(tx, trim_tx, channel, 15-power);

	xfer(tx, rx, packets);

	atrf_reg_write(tx, REG_TRX_STATE, TRX_CMD_TRX_OFF);
	atrf_reg_write(rx, REG_TRX_STATE, TRX_CMD_TRX_OFF);

	atrf_close(tx);
	atrf_close(rx);

	return 0;
}
