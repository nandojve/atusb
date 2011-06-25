/*
 * atrf-rssi/letter.h - Draw 7 segment style letters
 *
 * Written 2011 by Werner Almesberger
 * Copyright 2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef LETTER_H
#define LETTER_H

#include <stdint.h>

#include "SDL.h"


void letter(SDL_Surface *s, char c, int x0, int x1, int y0, int ym, int y1,
    uint32_t fg);

#endif /* !LETTER_H */
