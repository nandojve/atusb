/*
 * atrf-reset/atrf-reset.c - Reset the transceiver or the whole board
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
#include <string.h>

#include "atrf.h"


static void usage(const char *name)
{
	fprintf(stderr,
"usage: %s [-a|-t] [-d driver[:arg]]\n\n"
"  -a               reset MCU and transceiver\n"
"  -d driver[:arg]  use the specified driver (default: %s)\n"
"  -t               reset transceiver (default)\n"
    , name, atrf_default_driver_name());
	exit(1);
}


int main(int argc, char *const *argv)
{
	const char *driver = NULL;
	struct atrf_dsc *dsc;
	int txrx = 1;
	int c;

	while ((c = getopt(argc, argv, "ad:t")) != EOF)
		switch (c) {
		case 'a':
			txrx = 0;
			break;
		case 'd':
			driver = optarg;
			break;
		case 't':
			txrx = 1;
			break;
		default:
			usage(*argv);
		}
	if (argc != optind)
		usage(*argv);

	dsc = atrf_open(driver);
	if (!dsc)
		return 1;

        if (txrx)
                atrf_reset_rf(dsc);
        else
                atrf_reset(dsc);
        return 0;
}

