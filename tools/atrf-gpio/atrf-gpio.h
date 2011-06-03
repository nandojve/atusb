/*
 * atrf-gpio/atrf-gpio.h - ATBEN/ATUSB GPIO test
 *
 * Written 2011 by Werner Almesberger
 * Copyright 2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef ATRF_GPIO_H
#define	ATRF_GPIO_H

#include "atrf.h"


void do_atben(struct atrf_dsc *dsc, const char *pattern, const char *next);
void do_atusb(struct atrf_dsc *dsc, const char *pattern, const char *next);

#endif /* !ATRF_GPIO_H */
