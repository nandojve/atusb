/*
 * cntr/cntr.h - CNTR global variables
 *
 * Written 2010 by Werner Almesberger
 * Copyright 2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef CNTR_H
#define CNTR_H

/*
 * Free-running 32 bit counter. The lower two bytes are from hardware Timer 0.
 * The upper two bytes are maintained by software. At the maximum input clock
 * frequency of 6 MHz, it wraps around every 11.9 minutes, leaving the host
 * plenty of time to read it.
 */

extern uint8_t cntr[4];

extern enum hw_type {
	HW_TYPE_V1 = 0,
	HW_TYPE_V2 = 1,
} hw_type;

#endif /* !CNTR_H */
