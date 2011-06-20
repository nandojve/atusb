/*
 * include/misctxrx.h - Miscellaenous transceiver helper functions
 *
 * Written 2010-2011 by Werner Almesberger
 * Copyright 2010-2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef MISCTXRX_H
#define	MISCTXRX_H

#include <stdint.h>

#include "atrf.h"


void flush_interrupts(struct atrf_dsc *dsc);
uint8_t wait_for_interrupt(struct atrf_dsc *dsc, uint8_t wait_for,
    uint8_t ignore, int sleep_us, int timeout);

int tx_power_dBm2step(struct atrf_dsc *dsc, double power);
double tx_power_step2dBm(struct atrf_dsc *dsc, int step);

void set_power_step(struct atrf_dsc *dsc, int power, int crc);
void set_power_dBm(struct atrf_dsc *dsc, double power, int crc);

#endif /* !MISCTXRX_H */
