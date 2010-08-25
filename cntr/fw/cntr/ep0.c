/*
 * cntr/ep0.c - EP0 extension protocol
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
#include "usb.h"
#include "cntr/ep0.h"
#include "version.h"

#define debug(...)
#define error(...)


/*
 * SDCC 2.8.0 had a number of code generation bugs that appeared in the big
 * switch statement of my_setup. SDCC_FORCE_UPDATE forced the value of the
 * "size" variable to be written to memory. This work-around doesn't seem
 * to be necessary with 2.9.0, but we keep it around, just in case.
 *
 * Unfortunately, the setup->bRequest garbling bug is still with us. Without
 * the evaluation forced with SDCC_FORCE_EVAL, sdcc gets confused about the
 * value of setup->bRequest and then rejects all SETUP requests.
 */

#define	SDCC_FORCE_EVAL(type, value)	\
    do {				\
	static volatile type foo;	\
	foo = value;			\
    } while (0)

#define	SDCC_FORCE_UPDATE(type, var)	\
    do {				\
	volatile type foo;		\
	foo = var;			\
	var = foo;			\
    } while (0)


extern uint8_t cntr[8];

static const uint8_t id[] = { EP0CNTR_MAJOR, EP0CNTR_MINOR, HW_TYPE };
static __xdata uint8_t buf[128];


#define	BUILD_OFFSET	7	/* '#' plus "65535" plus ' ' */


/* crc32() */
#include "cntr/crc32.c"


static __bit my_setup(struct setup_request *setup) __reentrant
{
	uint32_t tmp;
	uint8_t size, i;

	switch (setup->bmRequestType | setup->bRequest << 8) {
	case CNTR_FROM_DEV(CNTR_ID):
		debug("CNTR_ID\n");
		if (setup->wLength > 3)
			return 0;
		usb_send(&ep0, id, setup->wLength, NULL, NULL);
		return 1;
	case CNTR_FROM_DEV(CNTR_BUILD):
		debug("CNTR_BUILD\n");
		tmp = build_number;
		for (i = BUILD_OFFSET-2; tmp; i--) {
			buf[i] = (tmp % 10)+'0';
			tmp /= 10;
		}
		buf[i] = '#';
		buf[BUILD_OFFSET-1] = ' ';
		for (size = 0; build_date[size]; size++)
			buf[BUILD_OFFSET+size] = build_date[size];
		size += BUILD_OFFSET-i+1;
		SDCC_FORCE_EVAL(uint8_t, setup->bRequest);
		if (size > setup->wLength)
			return 0;
		usb_send(&ep0, buf+i, size, NULL, NULL);
		return 1;

	case CNTR_TO_DEV(CNTR_RESET):
		debug("CNTR_RESET\n");
		RSTSRC = SWRSF;
		while (1);

	case CNTR_FROM_DEV(CNTR_READ):
		debug("CNTR_READ\n");
		buf[0] = cntr[0];
		buf[1] = cntr[1];
		buf[2] = cntr[2];
		buf[3] = cntr[3];
		tmp = (uint32_t) buf[0] | ((uint32_t) buf[1] << 8) |
		    ((uint32_t) buf[2] << 16) | ((uint32_t) buf[3] << 24);
		tmp = crc32(tmp, 0xffffffff);
		buf[4] = tmp;
		buf[5] = tmp >> 8;
		buf[6] = tmp >> 16;
		buf[7] = tmp >> 24;
		buf[8] = ~cntr[0];
		buf[9] = ~cntr[1];
		buf[10] = ~cntr[2];
		buf[11] = ~cntr[3];
		usb_send(&ep0, buf, 12, NULL, NULL);
		return 1;

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
