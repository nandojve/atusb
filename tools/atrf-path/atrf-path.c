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

#include "gui.h"
#include "sweep.h"


#define	DEFAULT_TRIM	8
#define	DEFAULT_POWER	15


static void set_channel(struct atrf_dsc *dsc, int chan)
{
	atrf_reg_write(dsc, REG_PHY_CC_CCA, (1 << CCA_MODE_SHIFT) | chan);
}


static void init_common(struct atrf_dsc *dsc, int trim)
{
	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_TRX_OFF);
	atrf_reg_write(dsc, REG_XOSC_CTRL,
	    (XTAL_MODE_INT << XTAL_MODE_SHIFT) | trim);
	atrf_set_clkm(dsc, 0);
}


static void init_tx(struct atrf_dsc *dsc, int trim, int power)
{
	init_common(dsc, trim);
	set_power_step(dsc, power, 0);
}


static void init_rx(struct atrf_dsc *dsc, int trim)
{
	init_common(dsc, trim);
	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_RX_ON);
}


static double rssi_to_dBm(double rssi)
{
	return -91+3*(rssi-1);
}


static void sample(const struct sweep *sweep, int cont_tx,
    struct sample *res, int first)
{
	int i, rssi;
	int sum = 0, min = -1, max = -1;
	double offset = tx_power_step2dBm(sweep->tx, sweep->power);

/*
 * For the 230, we don't have reset-less exit from test mode (yet ?) and
 * need to set up things from scratch:
 * 
 *	init_tx(sweep->tx, sweep->trim_tx, sweep->power);
 *	set_channel(sweep->tx, chan);
 *	usleep(155);    / * table 7-2, tTR19 * /
 */
	if (first)
		cw_test_begin(sweep->tx, cont_tx);
	else
		cw_test_resume(sweep->tx);
	/* table 7-1, tTR10, doubling since it's a "typical" value */
	usleep(2*16);

	for (i = 0; i != sweep->samples; i++) {
		/* according to 8.3.2, PHY_RSSI is updated every 2 us */
		usleep(2);

		rssi = atrf_reg_read(sweep->rx, REG_PHY_RSSI) & RSSI_MASK;
		sum += rssi;
		if (min == -1 || rssi < min)
			min = rssi;
		if (rssi > max)
			max = rssi;
	}

	cw_test_end(sweep->tx);

	res->avg = rssi_to_dBm((double) sum/sweep->samples)-offset;
	res->min = rssi_to_dBm(min)-offset;
	res->max = rssi_to_dBm(max)-offset;
}


void do_sweep(const struct sweep *sweep, struct sample *res)
{
	struct sample *r;
	int chan;

	r = res;
	for (chan = 11; chan <= 26; chan++) {
		set_channel(sweep->rx, chan);
		set_channel(sweep->tx, chan);
		usleep(155);	/* table 7-2, tTR19 */

		sample(sweep, CONT_TX_M500K, r, chan == 11);
		r += 2;
	}
	r = res+1;
	for (chan = 11; chan <= 26; chan++) {
		set_channel(sweep->rx, chan);
		set_channel(sweep->tx, chan);
		usleep(155);	/* table 7-2, tTR19 */

		sample(sweep, CONT_TX_P500K, r, chan == 11);
		r += 2;
	}
}


static void print_sweep(const struct sweep *sweep, const struct sample *res)
{
	int chan;

	for (chan = 11; chan <= 26; chan++) {
		printf("%.1f %.2f %.0f %.0f\n",
		    2350+5*chan-0.5, res->avg, res->min, res->max);
		res++;
		printf("%.1f %.2f %.0f %.0f\n",
		    2350+5*chan+0.5, res->avg, res->min, res->max);
		res++;
	}
}


static void do_sweeps(const struct sweep *sweep, int sweeps)
{
	struct sample res[16*2];	/* 16 channels, 2 offsets */
	int i;

	for (i = 0; i != sweeps; i++) {
		if (i)
			putchar('\n');
		do_sweep(sweep, res);
		print_sweep(sweep, res);
	}
}


static void usage(const char *name)
{
	fprintf(stderr,
"usage: %s [-p power] [-t trim_tx [-t trim_rx]] driver_tx[:arg]\n"
"%16s driver_rx[:arg] [sweeps [samples]]\n"

#ifdef HAVE_GFX
"%6s %s -g [-p power] [-t trim_tx [-t trim_rx]] driver_tx[:arg]\n"
"%16s driver_rx[:arg] [[sweeps] samples]\n"
#endif
    "\n"

#ifdef HAVE_GFX
"  -g        display results graphically\n"
#endif
"  -p power  transmit power, 0 to 15 (default %d)\n"
"  -t trim   trim capacitor, 0 to 15 (default %d)\n"

    , name, "",
#ifdef HAVE_GFX
    "", name, "",
#endif
    DEFAULT_POWER, DEFAULT_TRIM);

	exit(1);
}

	
int main(int argc, char **argv)
{
	const char *tx_drv, *rx_drv;
	struct sweep sweep = {
		.trim_tx = -1,
		.trim_rx = DEFAULT_TRIM,
		.samples = 1,
	};
	int graphical = 0;
	int power = DEFAULT_POWER;
	int sweeps = 1;
	unsigned long tmp;
	char *end;
	int c;

	while ((c = getopt(argc, argv, "gp:t:")) != EOF)
		switch (c) {
		case'g':
			graphical = 1;
			sweeps = 0;
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
			if (sweep.trim_tx == -1)
				sweep.trim_tx = tmp;
			else
				sweep.trim_rx = tmp;
			break;
		default:
			usage(*argv);
		}

	if (sweep.trim_tx == -1)
		sweep.trim_tx = DEFAULT_TRIM;

	switch (argc-optind) {
	case 4:
		sweep.samples = strtoul(argv[optind+3], &end, 0);
		if (*end)
			usage(*argv);
		/* fall through */
	case 3:
		sweeps = strtoul(argv[optind+2], &end, 0);
		if (*end)
			usage(*argv);
		if (graphical && argc-optind == 3) {
			sweep.samples = sweeps;
			sweeps = 0;
		}
		/* fall through */
	case 2:
		tx_drv = argv[optind];
		rx_drv = argv[optind+1];
		break;
	default:
		usage(*argv);
	}

	sweep.tx = atrf_open(tx_drv);
	if (!sweep.tx)
		return 1;
	sweep.rx = atrf_open(rx_drv);
	if (!sweep.rx)
		return 1;

	sweep.power = 15-power;
	init_rx(sweep.rx, sweep.trim_rx);
	init_tx(sweep.tx, sweep.trim_tx, sweep.power);
	if (graphical)
		gui(&sweep, sweeps);
	else
		do_sweeps(&sweep, sweeps);

	atrf_reg_write(sweep.tx, REG_TRX_STATE, TRX_CMD_TRX_OFF);
	atrf_reg_write(sweep.rx, REG_TRX_STATE, TRX_CMD_TRX_OFF);

	atrf_close(sweep.tx);
	atrf_close(sweep.rx);

	return 0;
}
