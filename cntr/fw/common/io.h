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
 * - v3: 330 Ohm termination
 */

#define	PROBE_TERM	P1_2

/*
 * Version ID
 *
 * In version 1, P2_1 is used to provide ground to the input side. In version
 * 2, we use a ground place for this and P2_1 is unconnected. We can therefore
 * use it to identify the hardware version.
 *
 * In version 3, P2_0 is connected to ground. In all other versions, it is
 * unconnected.
 *
 * Board  VERSION_ID1
 *        | VERSION_ID2
 *        | |
 * v1     0 Z
 * v2     Z Z
 * v3     Z 0
 * 
 */

#define	VERSION_ID1	P2_1
#define	VERSION_ID2	P2_0

#endif /* !IO_H */
