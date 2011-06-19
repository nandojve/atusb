/*
 * lib/atusb-common.h - ATUSB access functions shared by all ATUSB drivers
 *
 * Written 2010-2011 by Werner Almesberger
 * Copyright 2010-2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef ATUSB_COMMON_H
#define	ATUSB_COMMON_H


#define FROM_DEV        ATUSB_FROM_DEV(0)
#define TO_DEV          ATUSB_TO_DEV(0)


struct atusb_dsc {
	usb_dev_handle *dev;
	int error;
};


int atusb_error(void *handle);
int atusb_clear_error(void *handle);
void *atusb_open(const char *arg);
void atusb_close(void *handle);
void atusb_reset(void *handle);
void atusb_reset_rf(void *handle);
void atusb_test_mode(void *handle);
void atusb_slp_tr(void *handle, int on, int pulse);
int atusb_interrupt(void *handle);
int atusb_set_clkm(void *handle, int mhz);

#endif /* !ATUSB_COMMON_H */
