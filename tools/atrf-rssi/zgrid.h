/*
 * atspi-rssi/zgrid.h - Display a surface in faux 3D with SDL/SDL_gfx
 *
 * Written 2010 by Werner Almesberger
 * Copyright 2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef ZGRID_H
#define ZGRID_H

#include <stdint.h>

#include "SDL.h"


void zgrid_draw(SDL_Surface *s, const int *z, int nx, int ny,
    int sx, int sy, int sxy, int x0, int y0, uint32_t fg, uint32_t bg);

#endif /* !ZGRID_H */
