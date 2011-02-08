#include <stdint.h>

#include <avr/io.h>

#define F_CPU   8000000UL
#include <util/delay.h>

#include "freakusb.h"

#include "io.h"
#include "at86rf230.h"


static void spi_begin(void)
{
	CLR(nSS);
}


static uint8_t spi(uint8_t v)
{
//	while (!(UCSR1A & 1 << UDRE1));
	UDR1 = v;
	while (!(UCSR1A & 1 << RXC1));
	return UDR1;
}


static void spi_end(void)
{
//	while (!(UCSR1A & 1 << TXC1));
	SET(nSS);
}


int main(void)
{
	/* We start with a 1 MHz/8 clock. Disable the prescaler. */

	CLKPR = 1 << CLKPCE;
	CLKPR = 0;

	/* set up all the outputs; default port value is 0 */

	OUT(LED);
	OUT(nRST_RF);	/* reset the transceiver */
	OUT(SLP_TR);
	OUT(SCLK);
	OUT(MOSI);
	OUT(nSS);

	/* set up UART-SPI */

	UCSR1C = 1 << UMSEL11 | 1 << UMSEL10;
				/* set MSPI, MSB first, SPI data mode 0 */
	UCSR1B = 1 << RXEN1 | 1 << TXEN1;
				/* enable receiver and transmitter */
	UBRR1 = 0;		/* reconfirm the bit rate */

	/* bring the transceiver out of reset */

	/*
	 * AT86RF231 data sheet, 12.4.13, reset pulse with: 625 ns (min).
	 * We spend a lot more time getting here, so no extra wait is needed.
	 */
	SET(nRST_RF);

	/*
	 * 12.4.14: SPI access latency after reset: 625 ns
	 */
	_delay_us(1);

	/* switch CLKM to 8 MHz */

	/*
	 * @@@ Note: Atmel advise against changing the external clock in
	 * mid-flight. We should therefore switch to the RC clock first, then
	 * crank up the external clock, and finally switch back to the external
	 * clock. The clock switching procedure is described in the ATmega32U2
	 * data sheet in secton 8.2.2.
	 */

	spi_begin();
	spi(AT86RF230_REG_WRITE | REG_TRX_CTRL_0);
	spi(CLKM_CTRL_8MHz);
	spi_end();

	/* now we should be at 8 MHz */

	SET(LED);
	_delay_ms(100);
	CLR(LED);

	usb_init();
	hw_init();

	while (1)
		usb_poll();
}
