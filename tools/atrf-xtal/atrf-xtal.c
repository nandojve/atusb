/*
 * atrf-xtal/atrf-xtal.c - AT86RF230/1 crystal diagnostic utility
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

#include "atrf.h"

#include "atrf-xtal.h"


#define	DEFAULT_SIZE	127
#define	DEFAULT_TRIM	8


static void setup(struct atrf_dsc *dsc, int size, int trim)
{
#ifdef __mips__
	atben_setup(dsc, size, trim);
#else
#error	Only ATBEN is supported for now
#endif
}


unsigned sample(struct atrf_dsc *dsc)
{
#ifdef __mips__
	return atben_sample(dsc);
#else
#error	Only ATBEN is supported for now
#endif
}


void cleanup(struct atrf_dsc *dsc)
{
#ifdef __mips__
	atben_cleanup(dsc);
#else
#error	Only ATBEN is supported for now
#endif
}


static int cmp(const void *a, const void *b)
{
	return *(unsigned *) a-*(unsigned *) b;
}



static void eval(unsigned *res, int rep)
{
	double sum = 0;
	int n = 0;
	int i;

	qsort(res, rep, sizeof(*res), cmp);
	if (rep < 8) {
		printf("%u\n", res[rep >> 1]);
		return;
	}
	for (i = rep/8; i != rep-rep/8; i++) {
		sum += res[i];
		n++;
	}
	printf("%f\n", (double) sum/n);
}


static void usage(const char *name)
{
	fprintf(stderr,
"%s [-d] [-s size] [-t trim] [repetitions]\n"
"  -d           instead of printing a mean value, dump all samples\n"
"  -s size      payload size in bytes, 0-127 (default: %d bytes)\n"
"  -t trim      trim capacitor setting, 0-15 (default: %d)\n"
"  repetitions  number of measurements (default: 1)\n"
  , name, DEFAULT_SIZE, DEFAULT_TRIM);
	exit(1);
}


int main(int argc, char *const *argv)
{
	struct atrf_dsc *dsc;
	int size = DEFAULT_SIZE;
	int trim = DEFAULT_TRIM;
	int rep = 1;
	int dump = 0;
	char *end;
	unsigned *res;
	int c, i;

	while ((c = getopt(argc, argv, "ds:t:")) != EOF)
		switch (c) {
		case 'd':
			dump = 1;
			break;
		case 's':
			size = strtoul(optarg, &end, 0);
			if (*end)
				usage(*argv);
			if (size > 127)
				usage(*argv);
			break;
		case 't':
			trim = strtoul(optarg, &end, 0);
			if (*end)
				usage(*argv);
			if (trim > 15)
				usage(*argv);
			break;
		default:
			usage(*argv);
		}

	switch (argc-optind) {
	case 0:
		break;
	case 1:
		rep = strtoul(argv[optind], &end, 0);
		if (*end)
			usage(*argv);
		break;
	default:
		usage(*argv);
	}

	res = malloc(rep*sizeof(*res));
	if (!res) {
		perror("malloc");
		exit(1);
	}

	dsc = atrf_open();
	if (!dsc)
		return 1;

	setup(dsc, size, trim);

	for (i = 0; i != rep; i++)
		res[i] = sample(dsc);

	cleanup(dsc);

	atrf_close(dsc);

	if (dump) {
		for (i = 0; i != rep; i++)
			printf("%u\n", res[i]);
		exit(0);
	}

	eval(res, rep);
	
	return 0;
}
