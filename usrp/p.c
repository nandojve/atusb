#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>


#define PERC_DEFAULT	0.9


static int comp(const void *_a, const void *_b)
{
	float a = *(const float *) _a;
	float b = *(const float *) _b;

	return a < b ? -1 : a > b;
}


static void find_peak(int skip, float percentile, int dump)
{
	float max = 0;
	float c[2], a;
	float *rec = NULL;
	int e = 0, n = 0;

	while (1) {
		size_t s;

		s = fread(c, sizeof(c), 1, stdin);
		if (!s) {
			if (!ferror(stdin))
				break;
			if (s < 0) {
				perror("read");
				exit(1);
			}
		}
		if (skip) {
			skip--;
			continue;
		}
		a = hypotf(c[0], c[1]);
		if (a > max)
			max = a;
		if (e <= n) {
			e = e ? e*2 : 10000;
			rec = realloc(rec, e*sizeof(float));
			if (!rec) {
				perror("realloc");
				exit(1);
			}
		}
		rec[n] = a;
		n++;
	}

	if (skip >= n) {
		fprintf(stderr, "cannot skip %d of %d entries\n", skip, n);
		exit(1);
	}
	rec += skip;
	n -= skip;

	qsort(rec, n, sizeof(float), comp);

	if (!dump)
		printf("%f %f\n", max, rec[(int) (percentile*n)]);
	else {
		int i;

		for (i = 0; i < n; i += 1000)
			printf("%f %f\n", (double) i/n, rec[i]);
	}
}


static void usage(const char *name)
{
	fprintf(stderr,
"usage: %s [-s skip] [percentile]\n"
"       %s [-s skip] -d\n\n"
"  percentile  select the specified percentile (default: %g)\n\n"
"  -d          dump the histogram\n"
"  -s skip     skip this number of samples from the beginning (default: 0)\n"
    , name, name, PERC_DEFAULT);
	exit(1);
}


int main(int argc, char **argv)
{
	int dump = 0, skip = 0;
	float perc = PERC_DEFAULT;
	int c;

	while ((c = getopt(argc, argv, "ds:")) != EOF)
		switch (c) {
		case 'd':
			dump = 1;
			break;
		case 's':
			skip = atoi(optarg);
			break;
		default:
			usage(*argv);
		}

	switch (argc-optind) {
	case 0:
		break;
	case 1:
		if (dump)
			usage(*argv);
		perc = atof(argv[optind]);
		break;
	default:
		usage(*argv);
	}

	find_peak(skip, perc, dump);

	return 0;
}
