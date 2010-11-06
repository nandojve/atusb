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


#include <stdint.h>

#include "regs.h"
#include "io.h"
#include "usb.h"
#include "cntr/ep0.h"
#include "version.h"
#include "cntr.h"


uint8_t cntr[4];
enum hw_type hw_type;


static void delay(unsigned ms)
{
        int x;

	while (ms--)
	        for (x = 0; x < 1488; x)
	       	         x++;
}


static void init_io(void)
{
	int i;

	/* SDCC bug - needs parentheses here */
	hw_type = VERSION_ID1 ?
	    (VERSION_ID2 ? HW_TYPE_V2 : HW_TYPE_V3) :
	    HW_TYPE_V1;

	for (i = HW_TYPE_V1; i != hw_type; i++) {
		/* flash LED a second or third time */
		LEDv2 = 0;
		delay(250);
		LEDv2 = 1;
		delay(250);
	}
	if (hw_type == HW_TYPE_V2)
		PROBE_TERM_MODE |= 1 << PROBE_TERM_BIT;

	/*
	 * Signal	Mode		Value
	 *
	 * PROBE_T0	open drain	1	(input)
	 * PROBE_ECI	open drain	1	(input)
	 * PROBE_INT0	open drain	1	(input)
	 *
	 * PROBE_TERM	open drain	0	version 1
	 * PROBE_TERM	push-pull	1	version 2
	 *
	 * LEDv1	push-pull	0	(set up by boot loader)
	 * LEDv2	push-pull	0	(set up by boot loader)
	 *
	 * all unused	open drain 	0
	 */

	P0 = 1 << PROBE_INT0_BIT;
	P1 = (1 << PROBE_T0_BIT) | (1 << PROBE_ECI_BIT);
	P2 = 0;
	P3 = 0;

	if (hw_type == HW_TYPE_V2 || hw_type == HW_TYPE_V3)
		PROBE_TERM = 1;

	/*
	 * Disable pull-ups
	 */
	XBR1 |= WEAKPUD;
}


static void setup_counter(void)
{
	TCON = TR0;		/* enable Timer 0 */
	TMOD = C_T0 | T0M0;	/* clock from T0, mode 1: 16 bit counter */

	XBR1 |= T0E;		/* route T0 to port */

	P0SKIP = 0x7f;		/* assign T0 to P0_7 */
}


static void read_counter(void)
{
	uint8_t th;

	th = TH0;
	while (1) {
		cntr[0] = TL0;
		if (th == TH0)
			break;
		th = TH0;
	}
	if (th < cntr[1]) {
		cntr[2]++;
		if (!cntr[2])
			cntr[3]++;
	}
	cntr[1] = th;
}


void main(void)
{
	init_io();
	setup_counter();

	usb_init();
	ep0_init();

	while (1) {
		read_counter();
		usb_poll();
	}
}
