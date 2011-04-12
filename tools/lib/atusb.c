/*
 * lib/atusb.c - ATUSB access functions library (USB version)
 *
 * Written 2010-2011 by Werner Almesberger
 * Copyright 2010-2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdio.h>
#include <usb.h>

#include "atusb/ep0.h"
#include "atusb/usb-ids.h"

#include "usbopen.h"
#include "driver.h"


#define FROM_DEV	ATUSB_FROM_DEV(0)
#define TO_DEV		ATUSB_TO_DEV(0)


struct atusb_dsc {
	usb_dev_handle *dev;
	int error;
};

/* ----- error handling ---------------------------------------------------- */


static int atusb_error(void *handle)
{
	struct atusb_dsc *dsc = handle;

	return dsc->error;
}


static int atusb_clear_error(void *handle)
{
	struct atusb_dsc *dsc = handle;
	int ret;

	ret = dsc->error;
	dsc->error = 0;
	return ret;
}


/* ----- open/close -------------------------------------------------------- */


static void *atusb_open(const char *arg)
{
	usb_dev_handle *dev;
	struct atusb_dsc *dsc;

	usb_unrestrict();
	if (arg)
		restrict_usb_path(arg);
	dev = open_usb(USB_VENDOR, USB_PRODUCT);
	if (!dev) {
		fprintf(stderr, ":-(\n");
		return NULL;
	}

	dsc = malloc(sizeof(*dsc));
	if (!dsc) {
		perror("malloc");
		exit(1);
	}

	dsc->dev = dev;
	dsc->error = 0;

	return dsc;
}


static void atusb_close(void *handle)
{
	/* to do */
}


/* ----- device mode ------------------------------------------------------- */


static void atusb_reset(void *handle)
{
	struct atusb_dsc *dsc = handle;
	int res;

	if (dsc->error)
		return;

	res =
	    usb_control_msg(dsc->dev, TO_DEV, ATUSB_RESET, 0, 0, NULL, 0, 1000);
	if (res < 0) {
		fprintf(stderr, "ATUSB_RESET: %d\n", res);
		dsc->error = 1;
	}
}


static void atusb_reset_rf(void *handle)
{
	struct atusb_dsc *dsc = handle;
	int res;

	if (dsc->error)
		return;

	res = usb_control_msg(dsc->dev, TO_DEV, ATUSB_RF_RESET, 0, 0, NULL,
	    0, 1000);
	if (res < 0) {
		fprintf(stderr, "ATUSB_RF_RESET: %d\n", res);
		dsc->error = 1;
	}
}


static void atusb_test_mode(void *handle)
{
	struct atusb_dsc *dsc = handle;
	int res;

	if (dsc->error)
		return;

	res =
	    usb_control_msg(dsc->dev, TO_DEV, ATUSB_TEST, 0, 0, NULL, 0, 1000);
	if (res < 0) {
		fprintf(stderr, "ATUSB_TEST: %d\n", res);
		dsc->error = 1;
	}
}


/* ----- register access --------------------------------------------------- */


static void atusb_reg_write(void *handle, uint8_t reg, uint8_t value)
{
	struct atusb_dsc *dsc = handle;
	int res;

	if (dsc->error)
		return;

	res = usb_control_msg(dsc->dev, TO_DEV, ATUSB_REG_WRITE, value, reg,
	    NULL, 0, 1000);
	if (res < 0) {
		fprintf(stderr, "ATUSB_REG_WRITE: %d\n", res);
		dsc->error = 1;
	}
}


static uint8_t atusb_reg_read(void *handle, uint8_t reg)
{
	struct atusb_dsc *dsc = handle;
	uint8_t value = 0;
	int res;

	if (dsc->error)
		return 0;

	res = usb_control_msg(dsc->dev, FROM_DEV, ATUSB_REG_READ, 0, reg,
	    (void *) &value, 1, 1000);
	if (res < 0) {
		fprintf(stderr, "ATUSB_REG_READ: %d\n", res);
		dsc->error = 1;
	}
	return value;
}


/* ----- frame buffer access ----------------------------------------------- */


static void atusb_buf_write(void *handle, const void *buf, int size)
{
	struct atusb_dsc *dsc = handle;
	int res;

	if (dsc->error)
		return;

	res = usb_control_msg(dsc->dev, TO_DEV, ATUSB_BUF_WRITE, 0, 0,
	    (void *) buf, size, 1000);
	if (res < 0) {
		fprintf(stderr, "ATUSB_BUF_WRITE: %d\n", res);
		dsc->error = 1;
	}
}


static int atusb_buf_read(void *handle, void *buf, int size)
{
	struct atusb_dsc *dsc = handle;
	int res;

	if (dsc->error)
		return -1;

	res = usb_control_msg(dsc->dev, FROM_DEV, ATUSB_BUF_READ, 0, 0,
	    buf, size, 1000);
	if (res < 0) {
		fprintf(stderr, "ATUSB_BUF_READ: %d\n", res);
		dsc->error = 1;
	}

	return res;
}


/* ----- RF interrupt ------------------------------------------------------ */


static int atusb_interrupt(void *handle)
{
	struct atusb_dsc *dsc = handle;
	uint8_t buf;
	int res;

	if (dsc->error)
		return -1;
	
	res = usb_control_msg(dsc->dev, FROM_DEV, ATUSB_POLL_INT, 0, 0,
	    (void *) &buf, 1, 1000);
	if (res < 0) {
		fprintf(stderr, "ATUSB_POLL_INT: %d\n", res);
		dsc->error = 1;
		return -1;
	}

	return buf;
}


/* ----- CLKM handling ----------------------------------------------------- */


/*
 * ATmega32U2-based boards don't allow disabling CLKM, so we keep it at 8 MHz.
 * We could accommodate a choice between 8 MHz and 16 MHz, but that's for
 * later.
 */

static int atusb_set_clkm(void *handle, int mhz)
{
	struct atusb_dsc *dsc = handle;
	uint8_t ids[3];
	int res;

	if (dsc->error)
		return 0;
	res = usb_control_msg(dsc->dev, FROM_DEV, ATUSB_ID, 0, 0,
	    (void *) ids, 3, 1000);
	if (res < 0) {
		fprintf(stderr, "ATUSB_ID: %s\n", usb_strerror());
		dsc->error = 1;
		return 0;
	}
	switch (ids[2]) {
	case HW_TYPE_100813:
	case HW_TYPE_101216:
		break;
	case HW_TYPE_110131:
		if (mhz == 0 || mhz == 8)
			return 1;
		fprintf(stderr, "this board only supports CLKM = 8 MHz\n");
		return 0;
	default:
		fprintf(stderr,
		    "atusb_set_clkm: unknown hardware type 0x%02x\n", ids[2]);
		return 0;
	}
	return atrf_set_clkm_generic(atusb_reg_write, dsc, mhz);
}


/* ----- Driver-specific hacks --------------------------------------------- */


void *atusb_dev_handle(void *handle)
{
	struct atusb_dsc *dsc = handle;

	return dsc->dev;
}


/* ----- driver interface -------------------------------------------------- */


struct atrf_driver atusb_driver = {
	.name		= "usb",
	.open		= atusb_open,
	.close		= atusb_close,
	.error		= atusb_error,
	.clear_error	= atusb_clear_error,
	.reset		= atusb_reset,
	.reset_rf	= atusb_reset_rf,
	.test_mode	= atusb_test_mode,
	.slp_tr		= NULL,	/* @@@ not yet */
	.set_clkm	= atusb_set_clkm,
	.reg_write	= atusb_reg_write,
	.reg_read	= atusb_reg_read,
	.buf_write	= atusb_buf_write,
	.buf_read	= atusb_buf_read,
	.interrupt	= atusb_interrupt,
};
