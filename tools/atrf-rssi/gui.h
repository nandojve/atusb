/*
 * atrf-rssi/gui.h - Graphical output for atrf-rssi
 *
 * Written 2010-2011 by Werner Almesberger
 * Copyright 2010-2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef GUI_H
#define GUI_H

#include <stdlib.h>

#include "atrf.h"


#ifdef HAVE_GFX
void gui(struct atrf_dsc *dsc);
#else
#define gui(dsc) abort()
#endif

#endif /* !GUI_H */
