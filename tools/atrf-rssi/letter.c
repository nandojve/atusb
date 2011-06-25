/*
 * atrf-rssi/letter.c - Draw 7 segment style letters
 *
 * Written 2011 by Werner Almesberger
 * Copyright 2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdint.h>

#include "SDL.h"
#include "SDL_gfxPrimitives.h"

#include "letter.h"


static void hlines_3(SDL_Surface *s, int x0, int x1, int y0, int ym, int y1,
    uint32_t fg)
{
	hlineColor(s, x0, x1, y0, fg);
	hlineColor(s, x0, x1, ym, fg);
	hlineColor(s, x0, x1, y1, fg);
}


void letter(SDL_Surface *s, char c, int x0, int x1, int y0, int ym, int y1,
    uint32_t fg)
{
	switch (c) {
	case 'E':
		hlines_3(s, x0, x1, y0, ym, y1, fg);
		vlineColor(s, x0, y0, y1, fg);
		break;
	case 'U':
		vlineColor(s, x0, ym, y1, fg);
		/* fall through */
	case 'J':
		hlineColor(s, x0, x1, y0, fg);
		vlineColor(s, x1, y0, y1, fg);
		vlineColor(s, x0, y0, ym, fg);
		break;
	case 'P':
		hlineColor(s, x0, x1, y1, fg);
		hlineColor(s, x0, x1, ym, fg);
		vlineColor(s, x1, ym, y1, fg);
		vlineColor(s, x0, y0, y1, fg);
		break;
	case 'S':
		hlines_3(s, x0, x1, y0, ym, y1, fg);
		vlineColor(s, x0, ym, y1, fg);
		vlineColor(s, x1, y0, ym, fg);
		break;
		hlineColor(s, x0, x1, y0, fg);
		vlineColor(s, x0, y0, y1, fg);
	default:
		abort();
	}
}
