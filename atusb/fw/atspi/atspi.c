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


static int in_test_mode = 0;


static void set_test_mode(int on)
{
	in_test_mode = on;
	if (on) {
		TST_MODE |= 1 << TST_BIT;
		TST = 1;
		LED_MODE |= 1 << LED_BIT;
	} else {
		TST = 0;
		TST_MODE &= ~(1 << TST_BIT);
		LED_MODE &= ~(1 << LED_BIT);
		LED = 0;
	}
}


void reset_rf(void)
{
	int i;

	set_test_mode(0);

	nRST_RF = 0;
	/*
	 * 11.4.12: min 625 ns.
	 * The pulse we generate is slightly longer than 4 us.
	 */
	for (i = 0; i != 10; i++);
	nRST_RF = 1;
}


void test_mode(void)
{
	set_test_mode(1);
}


static void init_io(void)
{
	/*
	 * Signal	Mode		Value
	 *
	 * MOSI		push-pull	0
	 * MISO		open drain	1	(input)
	 * SCLK		push-pull	0
	 * nSS		push-pull	1
	 * nRST_RF	push-pull	1
	 * IRQ_RF	open drain	1	(input)
	 * SLP_TR	push-pull	0
	 * TST		open drain 	0
	 *
	 * LED		push-pull	0	(set up by boot loader)
	 *
	 * all unused	open drain 	0
	 */

	MOSI = 0;
	MOSI_MODE |= 1 << MOSI_BIT;

	SCLK = 0;
	SCLK_MODE |= 1 << SCLK_BIT;

	nSS_MODE |= 1 << nSS_BIT;

	nRST_RF_MODE |= 1 << nRST_RF_BIT;

	SLP_TR = 0;
	SLP_TR_MODE |= 1 << SLP_TR_BIT;

	P0 = 1;	/* IRQ_RF = 1; LED = 0; TST = Z; the rest is unused */
	P3 = 0;

#if 0
	/*
	 * We can *almost* disable the pull-ups. The only obstacle is that
	 * MISO is not driven when not in use. So we either need an external
	 * pull-up/down or keep all the pull-ups on.
	 */

	/*
	 * Disable pull-ups
	 */
	GPIOCN |= WEAKPUD;
#endif

	/*
	 * The manual says the reset is optional, but reality disagrees with
	 * this optimistic assessment quite violently.
	 */

	reset_rf();
}


void main(void)
{
	int i;

	init_io();

	/*
	 * Make sure the host has enough time (2.5 us) to detect that we reset
	 * our USB stack.
	 */
	for (i = 0; i != 10; i++);

	usb_init();
	ep0_init();

	while (1) {
		if (in_test_mode) {
			i++;
			LED = !(i >> 13);
		}
		usb_poll();
	}
}
