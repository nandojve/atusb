/*
 * atrf-rssi/zgrid.c - Display a surface in faux 3D with SDL/SDL_gfx
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
#include <stdlib.h>
#include <stdio.h>

#include "SDL.h"
#include "SDL_gfxPrimitives.h"

#include "zgrid.h"


#define	SWAP(x, y) 	\
	do { typeof(x) swap_tmp = (x); (x) = (y); (y) = swap_tmp; } while (0)


static int *alloc_row(int nx)
{
	int *p;

	p = malloc(nx*sizeof(*p));
	if (p)
		return p;
	perror("malloc");
	exit(1);
}


static void draw_band(SDL_Surface *s, const int *ax, const int *ay,
    const int *bx, const int *by, int n, uint32_t fg, uint32_t bg)
{
	Sint16 px[4], py[4];
	int i;

	for (i = n-2; i >= 0; i--) {
		px[0] = ax[i];
		px[1] = ax[i+1];
		px[2] = bx[i+1];
		px[3] = bx[i];
		py[0] = ay[i];
		py[1] = ay[i+1];
		py[2] = by[i+1];
		py[3] = by[i];
		filledPolygonColor(s, px, py, 4, bg);
		aalineColor(s, ax[i+1], ay[i+1], bx[i+1], by[i+1], fg);
		aalineColor(s, bx[i], by[i], bx[i+1], by[i+1], fg);
	}
	aalineColor(s, ax[0], ay[0], bx[0], by[0], fg);
}


static void draw_polyline(SDL_Surface *s, const int *px, const int *py,
    int n, uint32_t rgba)
{
	int i;

	for (i = 0; i != n-1; i++)
		aalineColor(s, px[i], py[i], px[i+1], py[i+1], rgba);
}


void zgrid_draw(SDL_Surface *s, const int *z, int nx, int ny,
    int sx, int sy, int sxy, int x0, int y0, uint32_t fg, uint32_t bg)
{
	int *lx = alloc_row(nx);
	int *ly = alloc_row(nx);
	int *px = alloc_row(nx);
	int *py = alloc_row(nx);
	int x, y, yz0;
	const int *zp;
	uint8_t a;

	for (y = ny-1; y >= 0; y--) {
		a = (ny-1-y)*0xe0/(ny-1)+0x1f;
		yz0 = s->h-y0-y*sy-1;
		zp = z+y*nx;
		for (x = 0; x != nx; x++) {
			px[x] = x0+x*sx+y*sxy;
			py[x] = yz0-zp[x];
		}
		if (y != ny-1) {
			draw_band(s, px, py, lx, ly, nx, fg | a, bg);
		}
		SWAP(px, lx);
		SWAP(py, ly);
	}
	draw_polyline(s, lx, ly, nx, fg | 0xff);
}
