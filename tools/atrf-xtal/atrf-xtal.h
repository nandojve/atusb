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


void atben_setup(struct atrf_dsc *dsc, int size, int trim);
unsigned atben_sample(struct atrf_dsc *dsc);
void atben_cleanup(struct atrf_dsc *dsc);

#endif /* !ATRF_XTAL_H */
