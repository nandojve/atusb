/*
 * fw/board_rzusb.c - RZUSB Board-specific functions (for boot loader and application)
 *
 * Written 2016 by Stefan Schmidt
 * Copyright 2016 Stefan Schmidt
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdbool.h>
#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/boot.h>

#define F_CPU   8000000UL
#include <util/delay.h>

#include "usb.h"
#include "at86rf230.h"
#include "board.h"
#include "spi.h"

void set_clkm(void)
{
	/* switch CLKM to 8 MHz */

	/*
	 * @@@ Note: Atmel advise against changing the external clock in
	 * mid-flight. We should therefore switch to the RC clock first, then
	 * crank up the external clock, and finally switch back to the external
	 * clock. The clock switching procedure is described in the ATmega32U2
	 * data sheet in secton 8.2.2.
	 */
	spi_begin();
	spi_send(AT86RF230_REG_WRITE | REG_TRX_CTRL_0);
	spi_send(0x10);
	spi_end();

	/* TX_AUTO_CRC_ON, default disabled */
	spi_begin();
	spi_send(AT86RF230_REG_WRITE | 0x05);
	spi_send(0x80);
	spi_end();
}

void board_init(void)
{
	/* Disable the watchdog timer */

	MCUSR = 0;		/* Remove override */
	WDTCSR |= 1 << WDCE;	/* Enable change */
	WDTCSR = 1 << WDCE;	/* Disable watchdog while still enabling
				   change */

	CLKPR = 1 << CLKPCE;
	/* We start with a 16 MHz/8 clock. Put the prescaler to 2. */
	CLKPR = 1 << CLKPS0;

	get_sernum();
}
