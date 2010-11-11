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

#define	LED	P0_1

/* SPI */

#define	MOSI	P2_2
#define	MISO	P2_5
#define	SCLK	P2_4
#define	nSS	P2_3

/* Miscellaneous RF signals */

#define	nRST_RF	P2_0
#define	IRQ_RF	P0_0
#define	SLP_TR	P2_1
#define	TST	P0_7

#endif /* !IO_H */
