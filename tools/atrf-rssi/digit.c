/*
 * atrf-rssi/digit.c - Draw 7 segment style digits
 *
 * Written 2010 by Werner Almesberger
 * Copyright 2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdint.h>

#include "SDL.h"
#include "SDL_gfxPrimitives.h"

#include "digit.h"


static void hlines_3(SDL_Surface *s, int x0, int x1, int y0, int ym, int y1,
    uint32_t fg)
{
	hlineColor(s, x0, x1, y0, fg);
	hlineColor(s, x0, x1, ym, fg);
	hlineColor(s, x0, x1, y1, fg);
}


void digit(SDL_Surface *s, int n, int x0, int x1, int y0, int ym, int y1,
    uint32_t fg)
{
	switch (n) {
	case 8:
		hlineColor(s, x0, x1, ym, fg);
		/* fall through */
	case 0:
		hlineColor(s, x0, x1, y0, fg);
		vlineColor(s, x0, y0, y1, fg);
		/* fall through */
	case 7:
		hlineColor(s, x0, x1, y1, fg);
		vlineColor(s, x1, y0, y1, fg);
		break;

	case 1:
		lineColor(s, x0, ym, x1, y1, fg);
		vlineColor(s, x1, y0, y1, fg);
		break;

	case 2:
		hlines_3(s, x0, x1, y0, ym, y1, fg);
		vlineColor(s, x0, y0, ym, fg);
		vlineColor(s, x1, ym, y1, fg);
		break;

	case 9:
		vlineColor(s, x0, ym, y1, fg);
		/* fall through */
	case 3:
		hlines_3(s, x0, x1, y0, ym, y1, fg);
		vlineColor(s, x1, y0, y1, fg);
		break;

	case 4:
		hlineColor(s, x0, x1, ym, fg);
		vlineColor(s, x0, ym, y1, fg);
		vlineColor(s, x1, y0, y1, fg);
		break;

	case 6:
		hlines_3(s, x0, x1, y0, ym, y1, fg);
		vlineColor(s, x0, y0, y1, fg);
		vlineColor(s, x1, y0, ym, fg);
		break;

	case 5:
		hlines_3(s, x0, x1, y0, ym, y1, fg);
		vlineColor(s, x0, ym, y1, fg);
		vlineColor(s, x1, y0, ym, fg);
		break;

	default:
		abort();
	}
}
