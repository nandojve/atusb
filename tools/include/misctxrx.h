/*
 * include/misctxrx.h - Miscellaenous transceiver helper functions
 *
 * Written 2010 by Werner Almesberger
 * Copyright 2010 Werner Almesberger
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


uint8_t wait_for_interrupt(struct atspi_dsc *dsc, uint8_t wait_for,
    uint8_t ignore, int sleep_us, int timeout);

#endif /* !MISCTXRX_H */
