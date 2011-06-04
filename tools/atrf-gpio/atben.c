/*
 * atrf-gpio/atben.c - ATBEN-specific GPIO driver
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
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "at86rf230.h"
#include "atrf.h"

#include "atrf-gpio.h"


static struct pin {
	const char *name;
	int bit;
} pins[] = {
	{ "SCLK",	11 },
	{ "MISO",	10 },
	{ "SLP_TR",	 9 },
	{ "MOSI",	 8 },
	{ "nSEL",	13 },
	{ "IRQ",	12 },
	{ NULL,		 0 }
};

static uint32_t orig_dat, orig_pe, orig_dir;
static uint32_t changed = 0;


/* ----- Ben hardware ------------------------------------------------------ */


static volatile uint32_t *pdpin, *pddat, *pddats, *pddatc;
static volatile uint32_t *pdpe, *pdpes, *pdpec;
static volatile uint32_t *pddir, *pddirs, *pddirc;


static void ben_setup(struct atrf_dsc *dsc)
{
	volatile void *base = atrf_ben_regs(dsc);

	pdpin = base+0x10300;
	pddat = base+0x10310;
	pddats = base+0x10314;
	pddatc = base+0x10318;

	pdpe = base+0x10330;
	pdpes = base+0x10334;
	pdpec = base+0x10338;

	pddir = base+0x10360;
	pddirs = base+0x10364;
	pddirc = base+0x10368;
}


/* ----- Diagnostic dump --------------------------------------------------- */


static char decode_cfg(uint32_t bit)
{
	if (*pddir & bit)
		return *pddat & bit ? '1' : '0';
	return *pdpe & bit ? 'Z' : 'R';
}


static void dump_pd(uint32_t expect, uint32_t got, uint32_t mask)
{
	const struct pin *pin;

	fprintf(stderr, "name\tcfg exp got\n");
	for (pin = pins; pin->name; pin++) {
		uint32_t bit = 1 << pin->bit;

		fprintf(stderr, "%s\t%c   %c   %d",
		    pin->name, decode_cfg(bit),
		    mask & bit ? expect & bit ? '1' : '0' : '-',
		    !!(got & bit));
		if ((expect ^ got) & mask & bit)
			fprintf(stderr, "\t***");
		fputc('\n', stderr);
	}
}


static void restore_gpios(void)
{
	*pddats = orig_dat & changed;
	*pddatc = ~orig_dat & changed;
	*pdpes = orig_pe & changed;
	*pdpec = ~orig_pe & changed;
	*pddirs = orig_dir & changed;
	*pddirc = ~orig_dir & changed;
}


/* ----- Decode and apply pattern ------------------------------------------ */


void do_atben(struct atrf_dsc *dsc, const char *pattern, const char *next)
{
	static int first = 1;
	uint32_t read = 0, expect = 0;
	const struct pin *pin = pins;
	uint32_t got;
	const char *p;

	if (first) {
		ben_setup(dsc);
		orig_dat = *pddat;
		orig_pe = *pdpe;
		orig_dir = *pddir;
		atexit(restore_gpios);

		first = 0;
	}

	for (p = pattern; *p; p++) {
		uint32_t bit;

		if (!pin->name) {
			fprintf(stderr, "too many pins in \"%s\"\n", pattern);
			exit(1);
		}
		bit = 1 << pin->bit;
		switch (*p) {
		case '0':
			*pddatc = bit;
			*pddirs = bit;
			break;
		case '1':
			*pddats = bit;
			*pddirs = bit;
			break;
		case 'H':
			expect |= bit;
			/* fall through */
		case 'L':
			read |= bit;
			/* fall through */
		case 'Z':
			*pdpec = bit;
			*pddirc = bit;
			break;
		case 'h':
			expect |= bit;
			/* fall through */
		case 'l':
			read |= bit;
			/* fall through */
		case 'z':
			*pddirc = bit;
			*pdpes = bit;
			break;
		case 'x':
			pin++;
			continue;
		default:
			continue;
		}
		changed |= bit;
		pin++;
	}

	usleep(1000);

	got = *pdpin;
	if ((got & read) != expect) {
		dump_pd(expect, got, read);
		fprintf(stderr, "at \"%s\", next \"%s\"\n", pattern, next);
		exit(1);
	}
}
