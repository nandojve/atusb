/*
 * atrf-txrx/perdump.h - Analyze and dump a recorded PER test
 *
 * Written 2011 by Werner Almesberger
 * Copyright 2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef PERDUMP_H
#define	PERDUMP_H

struct result_ops {
	void (*begin)(void);
	void (*undecided)(int symbols);
	void (*packet)(int symbols, int skip);
	void (*error)(int symbol);
	void (*finish)(void);
};


extern struct result_ops text_ops;

#endif /* !PERDUMP_H */
