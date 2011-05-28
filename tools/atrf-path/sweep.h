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


#define	N_CHAN	16

#define	MIN_DIFF	-97.0	/* RSSI(min)-TX(max) = -94 - 3		*/
#define	MAX_DIFF	7.0	/* RSSI(max)-TX(min) = -10 - (-17)	*/


struct sweep {
	struct atrf_dsc *tx;
	struct atrf_dsc *rx;
	int trim_tx;
	int trim_rx;
	int power;
	uint8_t cont_tx;
	int samples;
	double min[N_CHAN];
	double max[N_CHAN];
};

struct sample {
	double avg;
	double min, max;
};


/*
 * do_sweep returns whether the signal is within the limits:
 *
 *  1: at least one sample is above the maximum
 *  0: all samples are between minimum and maximum
 * -1: at least one sample below the minimum, and none above the maximum
 */

int do_sweep(const struct sweep *sweep, struct sample *res);

void print_sweep(const struct sweep *sweep, const struct sample *res);

#endif /* !SWEEP_H */
