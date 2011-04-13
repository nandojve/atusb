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
#include <string.h>
#include <ctype.h>

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
	static int need_init = 1;
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
	if (first || need_init) {
		cw_test_begin(sweep->tx, cont_tx);
		need_init = 0;
	} else {
		cw_test_resume(sweep->tx);
	}
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


static int do_half_sweep(const struct sweep *sweep, int cont_tx,
    struct sample *res)
{
	int i, chan;
	int fail = 0;

	if (sweep->cont_tx && sweep->cont_tx != cont_tx)
		return 0;
	chan = 11;
	for (i = 0; i != N_CHAN; i++) {
		set_channel(sweep->rx, chan);
		set_channel(sweep->tx, chan);
		usleep(155);	/* table 7-2, tTR19 */

		sample(sweep, cont_tx, res, chan == 11 && !sweep->cont_tx);
		if (res->avg > sweep->max[i])
			fail = 1;
		if (!fail && res->avg < sweep->min[i])
			fail = -1;

		res += 2;
		chan++;
	}
	return fail;
}


int do_sweep(const struct sweep *sweep, struct sample *res)
{
	int fail1, fail2;

	fail1 = do_half_sweep(sweep, CONT_TX_M500K, res);
	fail2 = do_half_sweep(sweep, CONT_TX_P500K, res+1);
	if (fail1 > 0 || fail2 > 0)
		return 1;
	if (fail1 < 0 || fail2 < 0)
		return -1;
	return 0;
}


static void print_sweep(const struct sweep *sweep, const struct sample *res)
{
	int chan;

	for (chan = 11; chan <= 26; chan++) {
		if (sweep->cont_tx != CONT_TX_P500K)
			printf("%.1f %.2f %.0f %.0f\n",
			    2350+5*chan-0.5, res->avg, res->min, res->max);
		res++;
		if (sweep->cont_tx != CONT_TX_M500K)
			printf("%.1f %.2f %.0f %.0f\n",
			    2350+5*chan+0.5, res->avg, res->min, res->max);
		res++;
	}
}


static int do_sweeps(const struct sweep *sweep, int sweeps)
{
	struct sample res[N_CHAN*2];	/* 2 offsets per channel */
	int decision = 0, fail, pass;
	int i;

	/*
	 * The pass/fail logic here goes as follows:
	 *
	 * Pass if and only if all sweeps pass.
	 * Fail if and only if all sweeps are below the minimum.
	 * Make no decision if any sweeps were above the maximum or if there
	 * was a mixture of pass and fail.
	 */

	for (i = 0; i != sweeps; i++) {
		if (i)
			putchar('\n');
		fail = do_sweep(sweep, res);
		print_sweep(sweep, res);
		pass = fail < 0 ? -1 : fail > 0 ? 0 : 1;
		if (!i)
			decision = pass;
		else {
			if (pass != decision)
				decision = 0;
		}
	}
	return decision;
}


static int do_read_profile(const char *name, struct sweep *sweep)
{
	FILE *file;
	char buf[300];
	int got;
	char *p;
	double min = MIN_DIFF, max = MAX_DIFF;
	int n = 0;

	file = fopen(name, "r");
	if (!file) {
		perror(name);
		exit(1);
	}
	while (fgets(buf, sizeof(buf), file)) {
		p = strchr(buf, '\n');
		if (p)
			*p = 0;
		p = strchr(buf, '#');
		if (p)
			*p = 0;
		for (p = buf; *p && isspace(*p); p++);
		if (!*p)
			continue;
		got = sscanf(buf, "%lf %lf", &min, &max);
		switch (got) {
		case 0:
			fprintf(stderr, "can't parse \"%s\"\n", buf);
			exit(1);
		case 1:
			max = MAX_DIFF;
			/* fall through */
		case 2:
			if (min < MIN_DIFF) {
				fprintf(stderr, "minimum is %g dBm\n",
				    MIN_DIFF);
				exit(1);
			}
			if (max > MAX_DIFF) {
				fprintf(stderr, "maximum is %g dBm\n",
				    MAX_DIFF);
				exit(1);
			}
			if (min > max) {
				fprintf(stderr, "lower bound > upper bound\n");
				exit(1);
			}
			if (n == N_CHAN) {
				fprintf(stderr, "too many channels\n");
				exit(1);
			}
			sweep->min[n] = min;
			sweep->max[n] = max;
			n++;
			break;
		default:
			abort();
		}
	}
	fclose(file);
	return n;
}


static void read_profile(const char *name, struct sweep *sweep)
{
	int n = 0;

	if (name)
		n = do_read_profile(name, sweep);

	while (n != N_CHAN) {
		sweep->min[n] = MIN_DIFF;
		sweep->max[n] = MAX_DIFF;
		n++;
	}
}


static void usage(const char *name)
{
	fprintf(stderr,
"usage: %s common_args [[sweeps] samples]\n"

#ifdef HAVE_GFX
"%6s %s -g common_args [[sweeps] samples]\n"
#endif
    "\n"
"  common args:  [-p power] [-P profile] [-t trim_tx [-t trim_rx]]\n"
"                [-T offset] driver_tx[:arg] driver_rx[:arg]\n\n"

#ifdef HAVE_GFX
"  -g          display results graphically\n"
#endif
"  -p power    transmit power, 0 to 15 (default %d)\n"
"  -P profile  load profile for pass/fail decisions\n"
"  -t trim     trim capacitor, 0 to 15 (default %d)\n"
"  -T offset   constant wave offset in MHz, -0.5 or +0.5 (default: scan both)\n"

    , name,
#ifdef HAVE_GFX
    "", name,
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
		.cont_tx = 0,
		.samples = 1,
	};
	int graphical = 0;
	int power = DEFAULT_POWER;
	const char *profile = NULL;
	int sweeps = 1;
	unsigned long tmp;
	char *end;
	int c, decision;

	while ((c = getopt(argc, argv, "gp:P:t:T:")) != EOF)
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
		case 'P':
			profile = optarg;
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
		case 'T':
			if (!strcmp(optarg, "-0.5"))
				sweep.cont_tx = CONT_TX_M500K;
			else if (!strcmp(optarg, "+0.5"))
				sweep.cont_tx = CONT_TX_P500K;
			else
				usage(*argv);
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
		if (argc-optind == 3) {
			sweep.samples = sweeps;
			sweeps = graphical ? 0 : 1;
		}
		/* fall through */
	case 2:
		tx_drv = argv[optind];
		rx_drv = argv[optind+1];
		break;
	default:
		usage(*argv);
	}

	read_profile(profile, &sweep);

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
		decision = gui(&sweep, sweeps);
	else
		decision = do_sweeps(&sweep, sweeps);

	switch (decision) {
	case -1:
		printf("#FAIL\n");
		break;
	case 0:
		break;
	case 1:
		printf("#PASS\n");
		break;
	default:
		abort();
	}

	atrf_reg_write(sweep.tx, REG_TRX_STATE, TRX_CMD_TRX_OFF);
	atrf_reg_write(sweep.rx, REG_TRX_STATE, TRX_CMD_TRX_OFF);

	atrf_close(sweep.tx);
	atrf_close(sweep.rx);

	return 0;
}
