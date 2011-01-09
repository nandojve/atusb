/*
 * atrf-rssi/atrf-rssi.c - ben-wpan AT86RF230 spectrum scan
 *
 * Written 2010-2011 by Werner Almesberger
 * Copyright 2010-2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

#include "at86rf230.h"
#include "atrf.h"
#include "misctxrx.h"

#ifdef HAVE_GFX
#include "gui.h"
#else
#define	gui(dsc) abort()
#endif


static struct timeval t0;
static volatile int run = 1;


static void sweep(struct atrf_dsc *dsc)
{
	int chan, rssi;
	struct timeval t;

	for (chan = 11; chan <= 26; chan++) {
		atrf_reg_write(dsc, REG_PHY_CC_CCA, chan);
		/* 150 us, according to AVR2001 section 3.5 */
		wait_for_interrupt(dsc, IRQ_PLL_LOCK, IRQ_PLL_LOCK, 10, 20);

		gettimeofday(&t, NULL);
		rssi = atrf_reg_read(dsc, REG_PHY_RSSI) & RSSI_MASK;
		t.tv_sec -= t0.tv_sec;
		t.tv_usec -= t0.tv_usec;
		printf("%d %f %d\n",
		    2405+(chan-11)*5,
		    (double) t.tv_sec+t.tv_usec/1000000.0,
		    -91+3*(rssi-1));
	}
	printf("\n");
}


static void die(int sig)
{
	run = 0;
}


static void usage(const char *name)
{
	fprintf(stderr,
"usage: %s [-n] sweeps\n", name);

#ifdef HAVE_GFX
	fprintf(stderr,
"%6s %s -g\n", "", name);
#endif

	exit(1);
}


int main(int argc, char **argv)
{
	struct atrf_dsc *dsc;
	unsigned long arg = 0, i;
	char *end;
	int c;
	int graphical = 0;

	while ((c = getopt(argc, argv, "gn")) != EOF)
		switch (c) {
#ifdef HAVE_GFX
		case 'g':
			graphical = 1;

			break;
#endif
		case 'n':
			break;
		default:
			usage(*argv);
		}

	switch (argc-optind) {
	case 0:
		if (!graphical)
			usage(*argv);
		break;
	case 1:
		if (graphical)
			usage(*argv);
		arg = strtoul(argv[optind], &end, 0);
		if (*end)
			usage(*argv);
		break;
	default:
		usage(*argv);
	}

	signal(SIGINT, die);

	dsc = atrf_open();
	if (!dsc)
		return 1;

	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_TRX_OFF);
	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_RX_ON);
	atrf_reg_write(dsc, REG_IRQ_MASK, IRQ_PLL_LOCK);
	/*
	 * We'll wait for the PLL lock after selecting the channel.
	 */

	if (graphical)
		gui(dsc);
	else {
		gettimeofday(&t0, NULL);
		for (i = 0; run && i != arg; i++)
			sweep(dsc);
	}

	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_TRX_OFF);

	atrf_close(dsc);

	return 0;
}
