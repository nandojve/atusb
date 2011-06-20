/*
 * lib/timeout.h - ATRF driver API
 *
 * Written 2011 by Werner Almesberger
 * Copyright 2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef TIMEOUT_H
#define	TIMEOUT_H

#include <sys/time.h>


struct timeout {
	struct timeval end;
};


void timeout_start(struct timeout *t, int ms);
int timeout_reached(const struct timeout *t);
int timeout_left_ms(const struct timeout *t);

#endif /* !TIMEOUT_H */
