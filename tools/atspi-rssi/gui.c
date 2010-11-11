/*
 * atspi-rssi/gui.c - Graphical output for atspi-rssi
 *
 * Written 2010 by Werner Almesberger
 * Copyright 2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

#include "SDL.h"
#include "SDL_gfxPrimitives.h"

#include "at86rf230.h"
#include "atrf.h"
#include "misctxrx.h"

#include "zgrid.h"
#include "digit.h"
#include "gui.h"


#define	XRES	320
#define	YRES	240

#define	N_CHAN	16
#define	N_TIME	64


#define	FG_RGBA		0xffffff00	/* grid foreround color */
#define	BG_RGBA		0x00408080	/* grid background color */
#define CHAN_RGBA	0xff4040ff	/* channel number color */
#define	FREQ_RGBA	0x20ff00ff	/* frequency color */

#define	X_STEP		17	/* grid x step */
#define	Y_STEP		 2	/* grid y step */
#define	Z_STEP		 3	/* z multiplier */
#define	X_STEP_Y	 1	/* x shift for each y step */
#define	X_OFFSET	 7	/* x coordinate of lower left grid corner */
#define	Y_OFFSET	40	/* y coordinate of lower left grid corner */


static struct timeval t0;


static void shift_grid(int *z, int nx, int ny)
{
	int *p1, *p0, *s;
	int x, y;

	p1 = z+(ny-1)*nx;
	for (y = 1; y != ny; y++) {
		p0 = s = p1-nx;
		for (x = 0; x != nx; x++)
			*p1++ = *p0++;
		p1 = s;
	}
}


static void sweep(struct atspi_dsc *dsc, int *z)
{
	int chan;

	for (chan = 11; chan <= 26; chan++) {
		atspi_reg_write(dsc, REG_PHY_CC_CCA, chan);
		/* 150 us, according to AVR2001 section 3.5 */
		wait_for_interrupt(dsc, IRQ_PLL_LOCK, IRQ_PLL_LOCK, 10, 20);

		*z++ = Z_STEP*atspi_reg_read(dsc, REG_PHY_RSSI) & RSSI_MASK;
#if 0
		if (chan >= 13 && chan <= 19 )
			z[-1] = 3*28-(chan-16)*(chan-16)*(chan-16)*(chan-16);
#endif
	}
}


static void clear(SDL_Surface *s)
{
	SDL_FillRect(s, NULL, SDL_MapRGB(s->format, 0, 0, 0));
}


#define	CBIG(pos) \
	x-5+(pos)*6, x-1+(pos)*6, y+8, y+4, y, CHAN_RGBA
#define	CSMALL(pos) \
	x-7+(pos)*4, x-5+(pos)*4, y+15, y+13, y+11, FREQ_RGBA


static void label_channels(SDL_Surface *s, int sx, int x0, int y0)
{
	int x, y, i, c, f;

	x = x0;
	y = s->h-y0+4;
	for (i = 0; i != N_CHAN; i++) {
		c = i+11;
		digit(s, c/10, CBIG(0));
		digit(s, c % 10, CBIG(1));
		f = 2405+5*i;
		if (i & 1)
			y++;
		digit(s, f/1000, CSMALL(0));
		digit(s, (f/100) % 10, CSMALL(1));
		digit(s, (f/10) % 10, CSMALL(2));
		digit(s, f % 10, CSMALL(3));
		if (i & 1)
			y--;
		x += sx;
	}
}


void gui(struct atspi_dsc *dsc)
{
	SDL_Surface *surf;
	int z[N_CHAN*N_TIME];
	SDL_Event event;

	memset(z, 0, sizeof(z));
	gettimeofday(&t0, NULL);
	
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
		while (SDL_PollEvent(&event))
			if (event.type == SDL_KEYDOWN ||
			    event.type == SDL_QUIT)
				return;

		shift_grid(z, N_CHAN, N_TIME);
		sweep(dsc, z);

		SDL_LockSurface(surf);

		clear(surf);
		zgrid_draw(surf, z, N_CHAN, N_TIME,
		    X_STEP, Y_STEP, X_STEP_Y,
		    X_OFFSET, Y_OFFSET,
		    FG_RGBA, BG_RGBA);
		label_channels(surf, X_STEP, X_OFFSET, Y_OFFSET);

		SDL_UnlockSurface(surf);
		SDL_UpdateRect(surf, 0, 0, 0, 0);
	}
}
