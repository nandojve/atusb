/*
 * atrf-path/gui.c - Graphical output for atrf-path
 *
 * Written 2011 by Werner Almesberger
 * Copyright 2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "SDL.h"
#include "SDL_gfxPrimitives.h"

#include "at86rf230.h"
#include "atrf.h"
#include "misctxrx.h"

#include "sweep.h"
#include "gui.h"


#define	XRES	320
#define	YRES	240

#define	N_CHAN	16


#define	FG_RGBA		0xffffffff	/* measurement color */

#define	CHAN_STEP	20	/* 4 pixels/MHz */
#define	SIDE_STEP	2
#define	CHAN_X_OFFSET	10
#define	Y_MIN		-94
#define	Y_MAX		-10


static void segment(SDL_Surface *s, int *last_x, int *last_y, int x,
    const struct sample *res, int have_last)
{
	int y = YRES-(res->avg-Y_MIN)/(Y_MAX-Y_MIN)*YRES-1;

	if (have_last) {
		aalineColor(s, *last_x, *last_y, x, y, FG_RGBA);
}
	*last_x = x;
	*last_y = y;
}


static void draw(SDL_Surface *s, const struct sample *res)
{
	int last_x, last_y;
	int x, i;

	x = CHAN_X_OFFSET;
	for (i = 0; i != N_CHAN; i++) {
		segment(s, &last_x, &last_y, x, res++, i);
		x += 2*SIDE_STEP;
		segment(s, &last_x, &last_y, x, res++, 1);
		x += CHAN_STEP-2*SIDE_STEP;
	}
}


static void clear(SDL_Surface *s)
{
	SDL_FillRect(s, NULL, SDL_MapRGB(s->format, 0, 0, 0));
}


void gui(const struct sweep *sweep)
{
	SDL_Surface *surf;
	SDL_Event event;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL_init: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);

	surf = SDL_SetVideoMode(XRES, YRES, 0, SDL_SWSURFACE);
	if (!surf) {
		fprintf(stderr, "SDL_SetVideoMode: %s\n", SDL_GetError());
		exit(1);
	}

	while (1) {
		struct sample res[N_CHAN*2];

		while (SDL_PollEvent(&event))
			if (event.type == SDL_KEYDOWN ||
			    event.type == SDL_QUIT)
				return;
		do_sweep(sweep, res);

		SDL_LockSurface(surf);

		clear(surf);
		draw(surf, res);

		SDL_UnlockSurface(surf);
		SDL_UpdateRect(surf, 0, 0, 0, 0);
	}
}
