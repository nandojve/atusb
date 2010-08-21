/*
 * atspi-txrx/atspi-txrx.c - ben-wpan AF86RF230 TX/RX
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
#include <usb.h>

#include "at86rf230.h"
#include "atspi/ep0.h"
#include "atspi.h"


#define	FROM_DEV	ATSPI_FROM_DEV(0)
#define	TO_DEV		ATSPI_TO_DEV(0)

#define	BUF_SIZE	256


static void usage(const char *name)
{
	fprintf(stderr, "%s\n", name);
	exit(1);
}


int main(int argc, const char **argv)
{
	usb_dev_handle *dev;

	if (argc != 1)
		usage(*argv);
	dev = atspi_open();
	if (!dev)
		return 1;

	atspi_reg_write(dev, REG_TRX_STATE, TRX_CMD_TRX_OFF);
	sleep(1000);

	return 0;
}
