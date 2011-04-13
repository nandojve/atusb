/*
 * atrf-path/sweep.h - Measure path characteristics
 *
 * Written 2011 by Werner Almesberger
 * Copyright 2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef	SWEEP_H
#define	SWEEP_H

#include <stdint.h>

#include "atrf.h"


struct sweep {
	struct atrf_dsc *tx;
	struct atrf_dsc *rx;
	int trim_tx;
	int trim_rx;
	int power;
	uint8_t cont_tx;
	int samples;
};

struct sample {
	double avg;
	double min, max;
};


void do_sweep(const struct sweep *sweep, struct sample *res);

#endif /* !SWEEP_H */
