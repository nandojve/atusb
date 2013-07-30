/*
 * fw/spi.c - ATmega8 family SPI I/O
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

#include "board.h"
#include "spi.h"


static bool spi_initialized = 0;


void spi_begin(void)
{
	if (!spi_initialized)
		spi_init();
	CLR(nSS);
}


uint8_t spi_io(uint8_t v)
{
//      while (!(UCSR1A & 1 << UDRE1));
        UDR1 = v;
        while (!(UCSR1A & 1 << RXC1));
        return UDR1;
}


void spi_end(void)
{
//      while (!(UCSR1A & 1 << TXC1));
	SET(nSS);
}


void spi_recv_block(uint8_t *buf, uint8_t n)
{
	if (!n)
		return;
        UDR1 = 0;
	while (--n) {
		while (!(UCSR1A & 1 << RXC1));
		*buf++ = UDR1;
		UDR1 = 0;
	}
	while (!(UCSR1A & 1 << RXC1));
	*buf++ = UDR1;
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
