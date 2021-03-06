/*
 * fw/board_atusb.c - ATUSB Board-specific functions (for boot loader and application)
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
#include "usb/usb.h"

static bool spi_initialized = 0;

void reset_rf(void)
{
	/* set up all the outputs; default port value is 0 */

	DDRB = 0;
	DDRC = 0;
	DDRD = 0;
	PORTB = 0;
	PORTC = 0;
	PORTD = 0;

	OUT(LED);
	OUT(nRST_RF);   /* this also resets the transceiver */
	OUT(SLP_TR);

	spi_init();

	/* AT86RF231 data sheet, 12.4.13, reset pulse width: 625 ns (min) */

	CLR(nRST_RF);
	_delay_us(2);
	SET(nRST_RF);

	/* 12.4.14: SPI access latency after reset: 625 ns (min) */

	_delay_us(2);

	/* we must restore TRX_CTRL_0 after each reset (9.6.4) */

	set_clkm();
}

void led(bool on)
{
	if (on)
		SET(LED);
	else
		CLR(LED);
}

void set_clkm(void)
{
	/* switch CLKM to 0 MHz */

	/*
	 * @@@ Note: Atmel advise against changing the external clock in
	 * mid-flight. We should therefore switch to the RC clock first, then
	 * crank up the external clock, and finally switch back to the external
	 * clock. The clock switching procedure is described in the ATmega32U2
	 * data sheet in secton 8.2.2.
	 */
	spi_begin();
	spi_send(AT86RF230_REG_WRITE | REG_TRX_CTRL_0);
	spi_send(CLKM_CTRL_OFF);
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
	/* We start with a 1 MHz/8 clock. Disable the prescaler. */
	CLKPR = 0;

	get_sernum();
}

void spi_begin(void)
{
	if (!spi_initialized)
		spi_init();
	CLR(nSS);
}

void spi_off(void)
{
	spi_initialized = 0;
	UCSR1B = 0;
}

void spi_init(void)
{
	SET(nSS);
	OUT(SCLK);
	OUT(MOSI);
	OUT(nSS);
	IN(MISO);

	UBRR1 = 0;	/* set bit rate to zero to begin */
	UCSR1C = 1 << UMSEL11 | 1 << UMSEL10;
			/* set MSPI, MSB first, SPI data mode 0 */
	UCSR1B = 1 << RXEN1 | 1 << TXEN1;
			/* enable receiver and transmitter */
	UBRR1 = 0;	/* reconfirm the bit rate */

	spi_initialized = 1;
}

void usb_init(void)
{
	UHWCON |= 1 << UVREGE;		/* Power-On USB pad regulator */
	USBCON |= 1 << FRZCLK;		/* freeze the clock */

	/* enable the PLL and wait for it to lock */
	PLLCSR &= ~(1 << PINDIV);
	PLLCSR |= 1 << PLLE;
	while (!(PLLCSR & (1 << PLOCK)));

	USBCON &= ~(1 << USBE);		/* reset the controller */
	USBCON |= 1 << USBE;

	USBCON &= ~(1 << FRZCLK);	/* thaw the clock */

	USBCON |= 1 << OTGPADE;

	while (!(USBSTA & (1 << VBUS)));

	UDCON &= ~(1 << DETACH);	/* attach the pull-up */
	UDIEN = 1 << EORSTE;		/* enable device interrupts  */
//	UDCON |= 1 << RSTCPU;		/* reset CPU on bus reset */

	ep_init();
}

void board_app_init(void)
{
	/* enable INT0, trigger on rising edge */
	EICRA = 1 << ISC01 | 1 << ISC00;
	EIMSK = 1 << 0;
}
