/*
 * lib/atusb.c - ATSPI access functions library (USB version)
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

#include "driver.h"


#define FROM_DEV	ATSPI_FROM_DEV(0)
#define TO_DEV		ATSPI_TO_DEV(0)


/* ----- error handling ---------------------------------------------------- */


static int error;


int atusb_error(void)
{
	return error;
}


int atusb_clear_error(void)
{
	int ret;

	ret = error;
	error = 0;
	return ret;
}


/* ----- open/close -------------------------------------------------------- */


static void *atusb_open(void)
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


static void atusb_close(void *dsc)
{
	/* to do */
}


/* ----- device mode ------------------------------------------------------- */


static void atusb_reset(void *dsc)
{
	usb_dev_handle *dev = dsc;
	int res;

	if (error)
		return;

	res =
	    usb_control_msg(dev, TO_DEV, ATSPI_RESET, 0, 0, NULL, 0, 1000);
	if (res < 0) {
		fprintf(stderr, "ATSPI_RESET: %d\n", res);
		error = 1;
	}
}


static void atusb_reset_rf(void *dsc)
{
	usb_dev_handle *dev = dsc;
	int res;

	if (error)
		return;

	res =
	    usb_control_msg(dev, TO_DEV, ATSPI_RF_RESET, 0, 0, NULL, 0, 1000);
	if (res < 0) {
		fprintf(stderr, "ATSPI_RF_RESET: %d\n", res);
		error = 1;
	}
}


/* ----- register access --------------------------------------------------- */


static void atusb_reg_write(void *dsc, uint8_t reg, uint8_t value)
{
	usb_dev_handle *dev = dsc;
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


static uint8_t atusb_reg_read(void *dsc, uint8_t reg)
{
	usb_dev_handle *dev = dsc;
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


/* ----- frame buffer access ----------------------------------------------- */


static void atusb_buf_write(void *dsc, const void *buf, int size)
{
	usb_dev_handle *dev = dsc;
	int res;

	if (error)
		return;

	res = usb_control_msg(dev, TO_DEV, ATSPI_BUF_WRITE, 0, 0,
	    (void *) buf, size, 1000);
	if (res < 0) {
		fprintf(stderr, "ATSPI_BUF_WRITE: %d\n", res);
		error = 1;
	}

}


static int atusb_buf_read(void *dsc, void *buf, int size)
{
	usb_dev_handle *dev = dsc;
	int res;

	if (error)
		return -1;

	res = usb_control_msg(dev, FROM_DEV, ATSPI_BUF_READ, 0, 0,
	    buf, size, 1000);
	if (res < 0) {
		fprintf(stderr, "ATSPI_BUF_READ: %d\n", res);
		error = 1;
	}

	return res;
}


/* ----- driver interface -------------------------------------------------- */


struct atspi_driver atusb_driver = {
	.name		= "atusb",
	.open		= atusb_open,
	.close		= atusb_close,
	.reset		= atusb_reset,
	.reset_rf	= atusb_reset_rf,
	.reg_write	= atusb_reg_write,
	.reg_read	= atusb_reg_read,
	.buf_write	= atusb_buf_write,
	.buf_read	= atusb_buf_read,
};
