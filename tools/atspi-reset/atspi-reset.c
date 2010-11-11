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
#include <string.h>

#include "atrf.h"


static void usage(const char *name)
{
	fprintf(stderr,
"usage: %s [-a|-t]\n\n"
"  -a  reset the MCU and transceiver\n"
"  -t  reset transceiver (default)\n"
    , name);
	exit(1);
}


int main(int argc, const char **argv)
{
	struct atspi_dsc *dsc;
	int txrx = 1;

	switch (argc) {
	case 1:
		break;
	case 2:
		if (!strcmp(argv[1], "-t"))
			break;
		txrx = 0;
		if (!strcmp(argv[1], "-a"))
			break;
		/* fall through */
	default:
		usage(*argv);
	}

	dsc = atspi_open();
	if (!dsc)
		return 1;

        if (txrx)
                atspi_reset_rf(dsc);
        else
                atspi_reset(dsc);
        return 0;
}

