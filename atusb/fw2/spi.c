#include <stdint.h>

#include <avr/io.h>

#include "board.h"
#include "spi.h"


void spi_begin(void)
{
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


void spi_init(void)
{
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
}
