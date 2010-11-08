/*
 * atspi-rssi/digit.h - Draw 7 segment style digits
 *
 * Written 2010 by Werner Almesberger
 * Copyright 2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef DIGIT_H
#define DIGIT_H

#include <stdint.h>

#include "SDL.h"


void digit(SDL_Surface *s, int n, int x0, int x1, int y0, int ym, int y1,
    uint32_t fg);

#endif /* !DIGIT_H */
