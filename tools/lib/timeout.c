/*
 * lib/timeout.c - Set up AT86RF230/231 constant wave test mode
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

#include "timeout.h"


void timeout_start(struct timeout *t, int ms)
{
	if (gettimeofday(&t->end, NULL) < 0) {
		perror("gettimeofday");
		exit(1);
	}
	t->end.tv_sec += ms/1000;
	t->end.tv_usec += 1000*(ms % 1000);
	if (t->end.tv_usec > 999999) {
		t->end.tv_sec++;
		t->end.tv_usec -= 1000000;
	}
}


int timeout_reached(const struct timeout *t)
{
	struct timeval now;

	if (gettimeofday(&now, NULL) < 0) {
		perror("gettimeofday");
		exit(1);
	}
	if (now.tv_sec > t->end.tv_sec)
		return 1;
	if (now.tv_sec < t->end.tv_sec)
		return 0;
	return now.tv_usec >= t->end.tv_usec;
}


int timeout_left_ms(const struct timeout *t)
{
	struct timeval now;
	int ms;

	if (gettimeofday(&now, NULL) < 0) {
		perror("gettimeofday");
		exit(1);
	}
	now.tv_sec = t->end.tv_sec-now.tv_sec;
	now.tv_usec = t->end.tv_usec-now.tv_usec;
	if (now.tv_usec < 0) {
		now.tv_sec--;
		now.tv_usec += 1000000;
	}
	return now.tv_sec*1000+now.tv_usec/1000;
}
