/*
 * lib/atspi.c - ATSPI access functions library
 *
 * Written 2010 by Werner Almesberger
 * Copyright 2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdio.h>
#include <usb.h>

#include "f32xbase/usb.h"
#include "atspi/ep0.h"
#include "atspi/usb-ids.h"

#include "atspi.h"


#define FROM_DEV	ATSPI_FROM_DEV(0)
#define TO_DEV		ATSPI_TO_DEV(0)


/* ----- error handling ---------------------------------------------------- */


static int error;


int atspi_error(void)
{
	return error;
}


int atspi_clear_error(void)
{
	int ret;

	ret = error;
	error = 0;
	return ret;
}


/* ----- open/close -------------------------------------------------------- */


usb_dev_handle *atspi_open(void)
{
	usb_dev_handle *dev;

	dev = open_usb(USB_VENDOR, USB_PRODUCT);
	if (dev) {
		error = 0;
	} else {
		fprintf(stderr, ":-(\n");
		error = 1;
	}
	return dev;
}


void atspi_close(usb_dev_handle *dev)
{
	/* to do */
}


/* ----- register access --------------------------------------------------- */


void atspi_reg_write(usb_dev_handle *dev, uint8_t reg, uint8_t value)
{
	int res;

	if (error)
		return;

	res = usb_control_msg(dev, TO_DEV, ATSPI_REG_WRITE, value, reg,
	    NULL, 0, 1000);
	if (res < 0) {
		fprintf(stderr, "ATSPI_REG_WRITE: %d\n", res);
		error = 1;
	}
}


uint8_t atspi_reg_read(usb_dev_handle *dev, uint8_t reg)
{
	uint8_t value = 0;
	int res;

	if (error)
		return 0;

	res = usb_control_msg(dev, FROM_DEV, ATSPI_REG_READ, 0, reg,
	    (void *) &value, 1, 1000);
	if (res < 0) {
		fprintf(stderr, "ATSPI_REG_READ: %d\n", res);
		error = 1;
	}
	return value;
}
