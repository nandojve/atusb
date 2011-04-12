/*
 * atrf-path/atrf-path.c - Measure path characteristics
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
#include "cwtest.h"
#include "atrf.h"


#define	DEFAULT_TRIM	8
#define	DEFAULT_POWER	15


static void init_common(struct atrf_dsc *dsc, int trim, int chan)
{
	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_TRX_OFF);
	atrf_reg_write(dsc, REG_XOSC_CTRL,
	    (XTAL_MODE_INT << XTAL_MODE_SHIFT) | trim);
	atrf_set_clkm(dsc, 0);
	atrf_reg_write(dsc, REG_PHY_CC_CCA, (1 << CCA_MODE_SHIFT) | chan);
}


static void init_tx(struct atrf_dsc *dsc, int trim, int power, int chan)
{
	init_common(dsc, trim, chan);
	set_power_step(dsc, power, 0);
}


static void init_rx(struct atrf_dsc *dsc, int trim, int chan)
{
	init_common(dsc, trim, chan);
	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_RX_ON);
}


static void sample(struct atrf_dsc *tx, struct atrf_dsc *rx, int trim_tx,
    int power, int chan, int cont_tx, int samples)
{
	int i, rssi = 0;
	double offset = tx_power_step2dBm(tx, power);

	init_tx(tx, trim_tx, power, chan);
	usleep(155);	/* table 7-2, tTR19 */

	cw_test_begin(tx, cont_tx);
	/* table 7-1, tTR10, doubling since it's a "typical" value */
	usleep(2*16);

	for (i = 0; i != samples; i++) {
		/* according to 8.3.2, PHY_RSSI is updated every 2 us */
		usleep(2);

		rssi += atrf_reg_read(rx, REG_PHY_RSSI) & RSSI_MASK;
	}

	cw_test_end(tx);

	printf("%.1f %.1f\n",
	    2350+5*chan+(cont_tx == CONT_TX_M500K ? -0.5 : 0.5),
	    -91+3*(rssi-1.0)/samples-offset);
}


static void do_sweeps(struct atrf_dsc *tx, struct atrf_dsc *rx,
    int trim_tx, int trim_rx, int power, int sweeps, int samples)
{
	int i, chan;

	for (i = 0; i != sweeps; i++) {
		if (i)
			putchar('\n');
		for (chan = 11; chan <= 26; chan++) {
			init_rx(rx, trim_rx, chan);

			sample(tx, rx, trim_tx, power, chan, CONT_TX_M500K,
			    samples);
			sample(tx, rx, trim_tx, power, chan, CONT_TX_P500K,
			    samples);
		}
	}
}


static void usage(const char *name)
{
	fprintf(stderr,
"usage: %s [-p power] [-t trim_tx [-t trim_rx]] driver_tx[:arg]\n"
"%16s driver_rx[:arg] [sweeps [samples]]\n\n"
"  -p power  transmit power, 0 to 15 (default %d)\n"
"  -t trim   trim capacitor, 0 to 15 (default %d)\n"
    , name, "", DEFAULT_POWER, DEFAULT_TRIM);
	exit(1);
}


int main(int argc, char **argv)
{
	const char *tx_drv, *rx_drv;
	struct atrf_dsc *tx, *rx;
	int trim_tx = -1, trim_rx = DEFAULT_TRIM;
	int power = DEFAULT_POWER;
	int sweeps = 1, samples = 1;
	unsigned long tmp;
	char *end;
	int c;

	while ((c = getopt(argc, argv, "p:t:")) != EOF)
		switch (c) {
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
	case 4:
		samples = strtoul(argv[optind+3], &end, 0);
		if (*end)
			usage(*argv);
		/* fall through */
	case 3:
		sweeps = strtoul(argv[optind+2], &end, 0);
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

	do_sweeps(tx, rx, trim_tx, trim_rx, 15-power, sweeps, samples);

	atrf_reg_write(tx, REG_TRX_STATE, TRX_CMD_TRX_OFF);
	atrf_reg_write(rx, REG_TRX_STATE, TRX_CMD_TRX_OFF);

	atrf_close(tx);
	atrf_close(rx);

	return 0;
}
