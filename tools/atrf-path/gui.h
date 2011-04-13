/*
 * atrf-path/gui.h - Graphical output for atrf-path
 *
 * Written 2011 by Werner Almesberger
 * Copyright 2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef GUI_H
#define GUI_H

#include <stdlib.h>

#include "sweep.h"


/*
 * gui returns one of the following values:
 *
 * -1	fail
 *  0	no decision taken
 *  1	pass
 */

#ifdef HAVE_GFX
int gui(const struct sweep *sweep, int sweeps);
#else
#define gui(sweep, sweeps) ({ abort(); 0; })
#endif

#endif /* !GUI_H */
