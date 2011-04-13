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

#define	FG_RGBA		0xffffffff	/* measurement color */
#define	OK_RGBA		0x00ff00ff
#define	OVER_RGBA	0xffff00ff
#define	UNDER_RGBA	0xff0000ff
#define	LIMIT_RGBA	0xff0000ff

#define	CHAN_STEP	20	/* 4 pixels/MHz */
#define	SIDE_STEP	2
#define	CHAN_X_OFFSET	10
#define	Y_MIN		-94
#define	Y_MAX		-10

#define	STATUS_X	(XRES-15)
#define	STATUS_Y	15
#define	STATUS_R	8


static int avg2y(double avg)
{
	return YRES-(avg-Y_MIN)/(Y_MAX-Y_MIN)*YRES-1;
}


static void segment(SDL_Surface *s, int *last_x, int *last_y, int x,
    const struct sample *res, int first)
{
	int y = avg2y(res->avg);

	if (!first) {
		aalineColor(s, *last_x, *last_y, x, y, FG_RGBA);
}
	*last_x = x;
	*last_y = y;
}


static void draw(SDL_Surface *s, const struct sample *res, int cont_tx)
{
	int last_x, last_y;
	int first, x, i;

	x = CHAN_X_OFFSET;
	first = 1;
	for (i = 0; i != N_CHAN; i++) {
		if (cont_tx != CONT_TX_P500K) {
			segment(s, &last_x, &last_y, x, res, first);
			first = 0;
		}
		res++;
		x += 2*SIDE_STEP;

		if (cont_tx != CONT_TX_M500K) {
			segment(s, &last_x, &last_y, x, res, first);
			first = 0;
		}
		res++;
		x += CHAN_STEP-2*SIDE_STEP;
	}
}


static void draw_limit(SDL_Surface *s, const double *v)
{
	int x, y, i, last = 0;

	x = CHAN_X_OFFSET;
	for (i = 0; i != N_CHAN; i++) {
		y = avg2y(*v);
		if (i)
			vlineColor(s, x-CHAN_STEP/2, last, y, LIMIT_RGBA);
		hlineColor(s, x-CHAN_STEP/2, x+CHAN_STEP/2, y, LIMIT_RGBA);
		last = y;
		x += CHAN_STEP;
		v++;
	}
}


static void indicate(SDL_Surface *s, int fail)
{
	static uint32_t last = 0;
	uint32_t color;

	switch (fail) {
	case 0:
		color = OK_RGBA;
		break;
	case 1:
		color = OVER_RGBA;
		break;
	case -1:
		color = UNDER_RGBA;
		break;
	default:
		abort();
	}
	if (color == last)
		color = 0;
	last = color;

	filledCircleColor(s, STATUS_X, STATUS_Y, STATUS_R, color);
	aacircleColor(s, STATUS_X, STATUS_Y, STATUS_R, color);
}


static void clear(SDL_Surface *s)
{
	SDL_FillRect(s, NULL, SDL_MapRGB(s->format, 0, 0, 0));
}


/* --- temporarily, for optimizing --- */

#include <sys/time.h>


static double t0;


static double t(void)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	return tv.tv_sec+tv.tv_usec/1000000.0;
}


static void tstart(void)
{
	t0 = t();
}


static void tstop(void)
{
	fprintf(stderr, "%.3f\n", t()-t0);
}


int gui(const struct sweep *sweep, int sweeps)
{
	SDL_Surface *surf;
	SDL_Event event;
	int cycle;
	int fail = 0;

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

	for (cycle = 0; cycle != sweeps || !sweeps; cycle++) {
		struct sample res[N_CHAN*2];

		/*
		 * Pass/fail logic:
		 *
		 * Quit  exit at any time, without making a pass/fail decision
		 * Pass  exit if the current result is "pass"
		 *       ignored if the current result is "over"/"under"
		 * Fail  exit if the current result is "under"
		 *       ignored if the current result is "pass"
		 *	 ignored if the current result is "over", because this
		 *         indicates an invalid measurement, not a defective
		 *         device
		 */

		while (SDL_PollEvent(&event))
			switch (event.type) {
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_f:
					if (cycle && fail < 0)
						return -1;
					break;
				case SDLK_p:
					if (cycle && !fail)
						return 1;
					break;
				case SDLK_q:
					return 0;
				default:
					break;
				}
				break;
			case SDL_QUIT:
				return 0;
			default:
				break;
			}

		tstart();
		fail = do_sweep(sweep, res);
		tstop();

		SDL_LockSurface(surf);

		clear(surf);

		draw_limit(surf, sweep->min);
		draw_limit(surf, sweep->max);
		indicate(surf, fail);
		draw(surf, res, sweep->cont_tx);

		SDL_UnlockSurface(surf);
		SDL_UpdateRect(surf, 0, 0, 0, 0);
	}

	return 0;
}
