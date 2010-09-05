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
#include <sys/time.h>

#include "at86rf230.h"
#include "atspi/ep0.h"
#include "atspi.h"


static struct timeval t0;


static void sweep(struct atspi_dsc *dsc)
{
	int chan, rssi;
	struct timeval t;

	for (chan = 11; chan <= 26; chan++) {
		atspi_reg_write(dsc, REG_PHY_CC_CCA, chan);
		/*
		 * No need to explicitly wait for the PPL lock - going USB-SPI
		 * is pretty slow, leaving the transceiver plenty of time.
		 */
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


static void usage(const char *name)
{
	fprintf(stderr, "%s sweeps \n", name);
	exit(1);
}


int main(int argc, const char **argv)
{
	struct atspi_dsc *dsc;
	unsigned long sweeps, i;
	char *end;

	if (argc != 2)
		usage(*argv);
	sweeps = strtoul(argv[1], &end, 0);
	if (*end)
		usage(*argv);

	dsc = atspi_open();
	if (!dsc)
		return 1;

	atspi_reg_write(dsc, REG_TRX_STATE, TRX_CMD_TRX_OFF);
	/*
	 * No need to explicitly wait for things to stabilize - going USB-SPI
	 * is pretty slow, leaving the transceiver more than enough time.
	 */
	atspi_reg_write(dsc, REG_TRX_STATE, TRX_CMD_RX_ON);

	gettimeofday(&t0, NULL);
	for (i = 0; i != sweeps; i++)
		sweep(dsc);

	atspi_reg_write(dsc, REG_TRX_STATE, TRX_CMD_TRX_OFF);

	return 0;
}
