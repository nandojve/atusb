/*
 * atrf-xtal/atusb.c - ATUSB-specific driver and evaluation
 *
 * Written 2011 by Werner Almesberger
 * Copyright 2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include <usb.h>
#include <gsl/gsl_fit.h>

#include "at86rf230.h"
#include "atusb/ep0.h"
#include "atrf.h"

#include "atrf-xtal.h"


#define FROM_DEV	ATUSB_FROM_DEV(0)

#define	F_NOMINAL_Hz	8000000	/* nominal frequency, in Hz */


static uint64_t get_sample(struct atrf_dsc *dsc, double *t0, double *t1)
{
	struct timeval tod0, tod1;
	uint64_t cnt;
	int res;

	gettimeofday(&tod0, NULL);
	res = usb_control_msg(atrf_usb_handle(dsc),
	    FROM_DEV, ATUSB_TIMER, 0, 0, (void *) &cnt, sizeof(cnt), 1000);
	gettimeofday(&tod1, NULL);
	if (res < 0) {
		fprintf(stderr, "ATUSB_TIMER: %s\n", usb_strerror());
		exit(1);
	}
	*t0 = tod0.tv_sec+tod0.tv_usec/1000000.0;
	*t1 = tod1.tv_sec+tod1.tv_usec/1000000.0;
	return cnt;
}


void do_atusb(struct atrf_dsc *dsc, int trim, int dump_raw, double ppm,
     int n)
{
	double t0[n], t1[n], cnt[n], w[n];
	double c0, c1, cov00, cov01, cov11, chisq;
	double d;
	double tz = 0, cntz = 0;
	double rel;
	int i;

	atrf_reg_write(dsc, REG_XOSC_CTRL,
	    (XTAL_MODE_INT << XTAL_MODE_SHIFT) | trim);

	for (i = 0; i != n; i++) {
		cnt[i] = get_sample(dsc, t0+i, t1+i);
		if (!i) {
			tz = t0[0];
			cntz = cnt[0];
		}
		t0[i] -= tz;
		t1[i] -= tz;
		cnt[i] -= cntz;

		/*
		 * see http://en.wikipedia.org/wiki/
		 * Uniform_distribution_(continuous)
		 */
		d = (t1[i]-t0[i])*F_NOMINAL_Hz;
		w[i] = 12/(d*d);
	}

	if (dump_raw) {
		for (i = 0; i != n; i++)
			printf("%.6f %.6f %.0f\n", t0[i], t1[i], cnt[i]);
		return;
	}

	gsl_fit_wlinear (t0, 1, w, 1, cnt, 1, n, 
	    &c0, &c1, &cov00, &cov01, &cov11, &chisq);

	rel = (c1/F_NOMINAL_Hz-1)*1000000;
	printf("%+.2f ppm", rel);
	if (ppm && fabs(rel) > ppm) {
		printf(" (outside bounds)\n");
		exit(1);
	}
	putchar('\n');
}
