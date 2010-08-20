/*
 * atspi/atspi.c - ATSPI initialization and main loop
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
#include "atspi/ep0.h"
#include "version.h"


static void init_io(void)
{
	/*
	 * Signal	Mode		Value
	 *
	 * MOSI		push-pull	0
	 * MISO		open drain	1	(input)
	 * SCLK		push-pull	1
	 * nSS		push-pull	1
	 * nRST_RF	push-pull	1
	 * IRQ_RF	open drain	1	(input)
	 * SLP_TR	push-pull	0
	 *
	 * LED		push-pull	0	(set up by boot loader)
	 *
	 * all unused	open drain 	0
	 */

	MOSI = 0;
	MOSI_MODE = 1;

	SCLK = 0;
	SCLK_MODE = 1;

	nSS_MODE = 1;

	nRST_RF_MODE = 1;

	SLP_TR = 0;
	SLP_TR_MODE = 1;

	P0 &=
	    ~((1 << 0) | (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6) | (1 << 7));
	P3 = 0;
	
}


void main(void)
{
	init_io();

	usb_init();
	ep0_init();

	while (1) {
		usb_poll();
	}
}
