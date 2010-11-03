/*
 * common/io.h - I/O pin assignment
 *
 * Written 2010 by Werner Almesberger
 * Copyright 2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef IO_H
#define IO_H

/* Diagnostic LED */

#define	LEDv1	P1_3
#define	LEDv2	P2_2

/* Probe input */

#define	PROBE_T0	P1_0
#define	PROBE_ECI	P1_1
#define	PROBE_INT0	P0_7

/*
 * Probe termination
 *
 * - v1: 100 kOhm termination (not very useful)
 * - v2: pull-up for input amplifier
 */

#define	PROBE_TERM	P1_2

#endif /* !IO_H */
