/*
 * cntr/cntr.c - CNTR initialization and main loop
 *
 * Written 2008-2010 by Werner Almesberger
 * Copyright 2008-2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "regs.h"
#include "io.h"
#include "usb.h"
#include "cntr/ep0.h"
#include "version.h"


static void init_io(void)
{
	/*
	 * Signal	Mode		Value
	 *
	 * PROBE_T0	open drain	1	(input)
	 * PROBE_ECI	open drain	1	(input)
	 * PROBE_INT0	open drain	1	(input)
	 *
	 * PROBE_TERM	open drain	0
	 *
	 * LED		push-pull	0	(set up by boot loader)
	 *
	 * all unused	open drain 	0
	 */

	P0 = 1 << PROBE_INT0_BIT;
	P1 = (1 << PROBE_T0_BIT) | (1 << PROBE_ECI_BIT);
	P2 = 0;
	P3 = 0;

	/*
	 * Disable pull-ups
	 */
	XBR1 |= WEAKPUD;
}


void main(void)
{
	init_io();

//	usb_init();
//	ep0_init();

	while (1) {
//		usb_poll();
	}
}
