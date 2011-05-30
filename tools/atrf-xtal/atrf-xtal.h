/*
 * atrf-xtal/atrf-xtal.h - AT86RF230/1 crystal diagnostic utility
 *
 * Written 2011 by Werner Almesberger
 * Copyright 2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef ATRF_XTAL_H
#define	ATRF_XTAL_H

#include "atrf.h"


void do_atben(struct atrf_dsc *dsc, int size, int trim, int rep,
    int dump_raw, double base, double ppm);
void do_atusb(struct atrf_dsc *dsc, int trim, int dump_raw, double ppm,
    int n);

#endif /* !ATRF_XTAL_H */
