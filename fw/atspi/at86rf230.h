/*
 * atspi/at86rf230.h - AT86RF230 protocol and register definitions
 *
 * Written 2008-2010 by Werner Almesberger
 * Copyright 2008-2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef AT86RF230_H
#define	AT86RF230_H

enum at86rf230_spi_cmd {
	AT86RF230_REG_WRITE	= 0xc0, /* 11... */
	AT86RF230_REG_READ	= 0x80,	/* 10... */
	AT86RF230_BUF_WRITE	= 0x60,	/* 011... */
	AT86RF230_BUF_READ	= 0x20,	/* 001... */
	AT86RF230_SRAM_WRITE	= 0x40,	/* 010... */
	AT86RF230_SRAM_READ	= 0x00 	/* 000... */
};

#define	MAX_PSDU	127	/* octets, see AT86RF230 manual section 8.1  */
#define	SRAM_SIZE	128

#endif /* !AT86RF230_H */
