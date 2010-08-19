/*
 * atspi/ep0.c - EP0 extension protocol
 *
 * Written 2008-2010 by Werner Almesberger
 * Copyright 2008-2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdint.h>

#ifndef NULL
#define NULL 0
#endif

#include "regs.h"
//#include "uart.h"
#include "usb.h"
#include "atspi/ep0.h"
#include "version.h"


#define debug(...)
#define error(...)


static const uint8_t id[] = { EP0ATSPI_MAJOR, EP0ATSPI_MINOR, HW_TYPE };


static __bit my_setup(struct setup_request *setup) __reentrant
{
	uint8_t size;

	switch (setup->bmRequestType | setup->bRequest << 8) {
	case ATSPI_FROM_DEV(ATSPI_ID):
		debug("ATSPI_ID\n");
		if (setup->wLength > 3)
			return 0;
		usb_send(&ep0, id, setup->wLength, NULL, NULL);
		return 1;
	case ATSPI_FROM_DEV(ATSPI_BUILD_NUMBER):
		debug("ATSPI_BUILD_NUMBER\n");
		if (setup->wLength > 2)
			return 0;
		usb_send(&ep0, (void *) &build_number, setup->wLength,
		    NULL, NULL);
		return 1;
	case ATSPI_FROM_DEV(ATSPI_BUILD_DATE):
		debug("ATSPI_BUILD_DATE\n");
		for (size = 0; build_date[size]; size++);
		if (size > EP1_SIZE)
			return 0;
		if (size > setup->wLength)
			return 0;
		usb_send(&ep0, build_date, size, NULL, NULL);
		return 1;
	case ATSPI_TO_DEV(ATSPI_RESET):
		debug("ATSPI_RESET\n");
		RSTSRC = SWRSF;
		while (1);

	default:
		error("Unrecognized SETUP: 0x%02x 0x%02x ...\n",
		    setup->bmRequestType, setup->bRequest);
		return 0;
	}
}


void ep0_init(void)
{
	user_setup = my_setup;
}
