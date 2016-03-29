/*
 * fw/board.c - Board-specific functions (for boot loader and application)
 *
 * Written 2011, 2013 by Werner Almesberger
 * Copyright 2011, 2013 Werner Almesberger
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


uint8_t board_sernum[42] = { 42, USB_DT_STRING };

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


void panic(void)
{
	cli();
	while (1) {
		SET(LED);
		_delay_ms(100);
		CLR(LED);
		_delay_ms(100);
	}
}


static char hex(uint8_t nibble)
{
	return nibble < 10 ? '0'+nibble : 'a'+nibble-10;
}


void get_sernum(void)
{
	uint8_t sig;
	uint8_t i;

	for (i = 0; i != 10; i++) {
		sig = boot_signature_byte_get(i+0xe);
		board_sernum[(i << 2)+2] = hex(sig >> 4);
		board_sernum[(i << 2)+4] = hex(sig & 0xf);
	}
}
