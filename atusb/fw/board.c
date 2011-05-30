/*
 * fw/board.c - Board-specific functions
 *
 * Written 2011 by Werner Almesberger
 * Copyright 2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


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


static uint32_t timer_h = 0;	/* 2^(16+32) / 8 MHz = ~1.1 years */


static void set_clkm(void)
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
	spi_send(CLKM_CTRL_8MHz);
	spi_end();
}


void reset_rf(void)
{
	/* AT86RF231 data sheet, 12.4.13, reset pulse width: 625 ns (min) */

	CLR(nRST_RF);
	_delay_us(1);
	SET(nRST_RF);

	/* 12.4.14: SPI access latency after reset: 625 ns (min) */

	_delay_us(1);

	/* we must restore TRX_CTRL_0 after each reset (9.6.4) */

	set_clkm();
}


void reset_cpu(void)
{
	WDTCSR = 1 << WDE;
}


uint8_t read_irq(void)
{
	return PIN(IRQ_RF);
}


void led(int on)
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


void timer_poll(void)
{
	if (!(TIFR1 & (1 << TOV1)))
		return;
	TIFR1 = 1 << TOV1;
	timer_h++;
}


uint64_t timer_read(void)
{
	uint32_t high;
	uint8_t low, mid;

	do {
		timer_poll();
		high = timer_h;
		low = TCNT1L;
		mid = TCNT1H;
	}
	while (TIFR1 & (1 << TOV1));

	/*
	 * We need all these casts because the intermediate results are handled
	 * as if they were signed and thus get sign-expanded. Sounds wrong-ish.
	 */
	return (uint64_t) high << 16 | (uint64_t) mid << 8 | (uint64_t) low;
}


static char hex(uint8_t nibble)
{
	return nibble < 10 ? '0'+nibble : 'a'+nibble-10;
}


static void get_sernum(void)
{
	uint8_t sig;
	int i;

	for (i = 0; i != 10; i++) {
		sig = boot_signature_byte_get(i+0xe);
		board_sernum[(i << 2)+2] = hex(sig >> 4);
		board_sernum[(i << 2)+4] = hex(sig & 0xf);
	}
}


void board_init(void)
{
	/* Disable the watchdog timer */

	MCUSR = 0;		/* Remove override */
	WDTCSR |= 1 << WDCE;	/* Enable change */
	WDTCSR = 1 << WDCE;	/* Disable watchdog while still enabling
				   change */

	/* We start with a 1 MHz/8 clock. Disable the prescaler. */

	CLKPR = 1 << CLKPCE;
	CLKPR = 0;

	/* set up all the outputs; default port value is 0 */

	OUT(LED);
	OUT(nRST_RF);   /* this also resets the transceiver */
	OUT(SLP_TR);

	/* configure timer 1 as a free-running CLK counter */

	TCCR1A = 0;
	TCCR1B = 1 << CS10;

	get_sernum();
}
