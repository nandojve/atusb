/*
 * atspi-rssi/atspi-rssi.c - ben-wpan AT86RF230 spectrum scan
 *
 * Written 2010 by Werner Almesberger
 * Copyright 2010 Werner Almesberger
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
#include "atspi.h"
#include "misctxrx.h"


static struct timeval t0;
static volatile int run = 1;


static void sweep(struct atspi_dsc *dsc)
{
	int chan, rssi;
	struct timeval t;

	for (chan = 11; chan <= 26; chan++) {
		atspi_reg_write(dsc, REG_PHY_CC_CCA, chan);
		/* 150 us, according to AVR2001 section 3.5 */
		wait_for_interrupt(dsc, IRQ_PLL_LOCK, IRQ_PLL_LOCK, 10, 20);

		gettimeofday(&t, NULL);
		rssi = atspi_reg_read(dsc, REG_PHY_RSSI) & RSSI_MASK;
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
	exit(1);
}


int main(int argc, char **argv)
{
	struct atspi_dsc *dsc;
	unsigned long sweeps, i;
	char *end;
	int c;

	while ((c = getopt(argc, argv, "n")) != EOF)
		switch (c) {
		case 'n':
			break;
		default:
			usage(*argv);
		}

	switch (argc-optind) {
	case 1:
		sweeps = strtoul(argv[optind], &end, 0);
		if (*end)
			usage(*argv);
		break;
	default:
		usage(*argv);
	}

	signal(SIGINT, die);

	dsc = atspi_open();
	if (!dsc)
		return 1;

	atspi_reg_write(dsc, REG_TRX_STATE, TRX_CMD_TRX_OFF);
	atspi_reg_write(dsc, REG_TRX_STATE, TRX_CMD_RX_ON);
	/*
	 * We'll wait for the PLL lock after selecting the channel.
	 */

	gettimeofday(&t0, NULL);
	for (i = 0; run && i != sweeps; i++)
		sweep(dsc);

	atspi_reg_write(dsc, REG_TRX_STATE, TRX_CMD_TRX_OFF);

	atspi_close(dsc);

	return 0;
}
