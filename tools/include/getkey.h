/*
 * include/getkey.h - Get single characters from standard input
 *
 * Written 2011 by Werner Almesberger
 * Copyright 2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef GETKEY_H
#define	GETKEY_H

void get_key_init(void);
char get_key(void);

#endif /* !GETKEY_H */
