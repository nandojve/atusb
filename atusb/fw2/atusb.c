#include <stdint.h>

#include <avr/io.h>

#define F_CPU   8000000UL
#include <util/delay.h>

#include "freakusb.h"

#include "board.h"
#include "spi.h"
#include "atusb/ep0.h"


int main(void)
{
	board_init();
	spi_init();
	reset_rf();

	/* now we should be at 8 MHz */

	led(1);
	_delay_ms(100);
	led(0);

	usb_init();
	ep0_init();
	hw_init();

	while (1)
		usb_poll();
}
