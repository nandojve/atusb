#include <stdint.h>

#include <avr/io.h>

#define F_CPU   8000000UL
#include <util/delay.h>

#include "freakusb.h"

#include "at86rf230.h"
#include "board.h"
#include "spi.h"
#include "atusb/ep0.h"


int main(void)
{
	/* We start with a 1 MHz/8 clock. Disable the prescaler. */

	CLKPR = 1 << CLKPCE;
	CLKPR = 0;

	/* set up all the outputs; default port value is 0 */

	OUT(LED);
	OUT(nRST_RF);	/* resets the transceiver */
	OUT(SLP_TR);

	spi_init();

	reset_rf();

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

	/* now we should be at 8 MHz */

	SET(LED);
	_delay_ms(100);
	CLR(LED);

	usb_init();
	ep0_init();
	hw_init();

	while (1)
		usb_poll();
}
