/*
 * atspi/atspi.c - ATSPIinitialization and main loop
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


void main(void)
{
	usb_init();
	ep0_init();

	while (1) {
		usb_poll();
	}
}
