/*
 * atspi-trim/atspi-trim.c - AT86RF230 oscillator trim utility
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
#include "atrf.h"


static void usage(const char *name)
{
	fprintf(stderr, "%s [trim_value]\n", name);
	exit(1);
}


int main(int argc, const char **argv)
{
	struct atrf_dsc *dsc;
	int trim = -1;
	char *end;

	switch (argc) {
	case 1:
		break;
	case 2:
		trim = strtoul(argv[1], &end, 0);
		if (*end || trim > 15)
			usage(*argv);
		break;
	default:
		usage(*argv);
	}

	dsc = atrf_open();
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
