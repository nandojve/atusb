/*
 * atrf-txrx/per-text.h - Report PER on console as text
 *
 * Written 2011 by Werner Almesberger
 * Copyright 2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdio.h>

#include "perdump.h"


static char line[65] = "";
static int packets = 0, garbled = 0, bad = 0, skipped = 0;
static int is_bad = 0;


static void flush(void)
{
	if (*line)
		printf("%s\n", line);
	*line = 0;
}


static void text_undecided(int symbols)
{
	int i;

	flush();
	for (i = 0; i != symbols/4; i++)
		putchar('?');
	putchar('\n');
}


static void text_packet(int symbols, int skip)
{
	int i;

	flush();
	skipped += skip;
	if (skip < 4)
		for (i = 0; i != skip; i++)
			putchar('\n');
	else
		printf("\n(%d)\n\n", skip);
	for (i = 0; i != symbols/4; i++)
		line[i] = '-';
	line[i] = 0;
	packets++;
	is_bad = 0;
}


static void text_error(int symbol)
{
	line[symbol >> 2] = '*';
	if (!is_bad) {
		bad++;
		is_bad = 1;
	}
}


static void text_finish(void)
{
	double per;

	flush();
	if (packets+garbled)
		per = (double) (bad+garbled)/(packets+garbled);
	else
		per = 0;
	printf("\n%d total, %d bad, %d garbled, PER %f%%. %d skipped.\n",
	    packets+garbled, bad, garbled, 100*per, skipped);
}


struct result_ops text_ops = {
	.begin		= NULL,
	.undecided	= text_undecided,
	.packet		= text_packet,
	.error		= text_error,
	.finish		= text_finish,
};
