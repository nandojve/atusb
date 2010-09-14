/*
 * lib/misctxrx.c - Miscellaenous transceiver helper functions
 *
 * Written 2010 by Werner Almesberger
 * Copyright 2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include "at86rf230.h"
#include "atspi.h"
#include "misctxrx.h"


static volatile int run = 1;


static void die(int sig)
{
	run = 0;
}


uint8_t wait_for_interrupt(struct atspi_dsc *dsc, uint8_t wait_for,
    uint8_t ignore, int sleep_us, int timeout)
{
	uint8_t irq = 0, show;
	void (*old_sig)(int);

	run = 1;
	old_sig = signal(SIGINT, die);
	while (run) {
		while (run && !atspi_interrupt(dsc)) {
			usleep(sleep_us);
			if (timeout && !--timeout)
				return 0;
		}
		irq = atspi_reg_read(dsc, REG_IRQ_STATUS);
		if (atspi_error(dsc))
			exit(1);
		if (!irq)
			continue;
		show = irq & ~ignore;
		if ((irq & wait_for) && !show)
			break;
		fprintf(stderr, "IRQ (0x%02x):", irq);
		if (irq & IRQ_PLL_LOCK)
			fprintf(stderr, " PLL_LOCK");
		if (irq & IRQ_PLL_UNLOCK)
			fprintf(stderr, " PLL_UNLOCK");
		if (irq & IRQ_RX_START)
			fprintf(stderr, " RX_START");
		if (irq & IRQ_TRX_END)
			fprintf(stderr, " TRX_END");
		if (irq & IRQ_TRX_UR)
			fprintf(stderr, " TRX_UR");
		if (irq & IRQ_BAT_LOW)
			fprintf(stderr, " BAT_LOW");
		fprintf(stderr, "\n");
		if (irq & wait_for)
			break;
	}
	signal(SIGINT, old_sig);
	if (!run)
		raise(SIGINT);
	return irq;
}
