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
	SPI_DATA = v;
	SPI_WAIT_DONE();
	return SPDR;
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
	SPI_DATA = 0;
	while (--n) {
		SPI_WAIT_DONE();
		*buf++ = SPI_DATA;
		SPI_DATA = 0;
	}
	SPI_WAIT_DONE();
	*buf++ = SPI_DATA;
}


void spi_off(void)
{
	spi_initialized = 0;
	#ifdef ATUSB
		UCSR1B = 0;
	#endif
	#ifdef RZUSB
		SPCR &= ~(1 << SPE);
	#endif
}


void spi_init(void)
{
	SET(nSS);
	OUT(SCLK);
	OUT(MOSI);
	OUT(nSS);
	IN(MISO);

#ifdef ATUSB
	UBRR1 = 0;	/* set bit rate to zero to begin */
	UCSR1C = 1 << UMSEL11 | 1 << UMSEL10;
			/* set MSPI, MSB first, SPI data mode 0 */
	UCSR1B = 1 << RXEN1 | 1 << TXEN1;
			/* enable receiver and transmitter */
	UBRR1 = 0;	/* reconfirm the bit rate */
#endif
#ifdef RZUSB
	SPCR = (1 << SPE) | (1 << MSTR);
	SPSR = (1 << SPI2X);
#endif

	spi_initialized = 1;
}
