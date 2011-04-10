/*
 * atrf-trim/atrf-trim.c - AT86RF230 oscillator trim utility
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
#include <sys/time.h>

#include "at86rf230.h"
#include "atrf.h"


static void usage(const char *name)
{
	fprintf(stderr, "%s [-d driver[:arg]] [trim_value]\n", name);
	exit(1);
}


int main(int argc, char *const *argv)
{
	const char *driver = NULL;
	struct atrf_dsc *dsc;
	int trim = -1;
	char *end;
	int c;

	while ((c = getopt(argc, argv, "d:")) != EOF)
		switch (c) {
		case 'd':
			driver = optarg;
			break;
		default:
			usage(*argv);
		}

	switch (argc-optind) {
	case 0:
		break;
	case 1:
		trim = strtoul(argv[optind], &end, 0);
		if (*end || trim > 15)
			usage(*argv);
		break;
	default:
		usage(*argv);
	}

	dsc = atrf_open(driver);
	if (!dsc)
		return 1;

	if (trim == -1) {
		trim = atrf_reg_read(dsc, REG_XOSC_CTRL) & XTAL_TRIM_MASK;
		printf("%d (%d.%d pF)\n", trim, trim*3/10, trim*3 % 10);
	} else {
		atrf_reg_write(dsc, REG_XOSC_CTRL,
		    (XTAL_MODE_INT << XTAL_MODE_SHIFT) | trim);
	}

	return 0;
}
