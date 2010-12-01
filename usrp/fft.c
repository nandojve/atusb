#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>

#include <fftw3.h>


#define  DEFAULT_THRESHOLD	100

static int alg = 0;


static void fft_complex(int n, const float *re, const float *im, double *res)
{
	fftw_complex *in, *out;
	fftw_plan plan;
	int i;
	double a;

	in = fftw_malloc(sizeof(fftw_complex)*n);
	out = fftw_malloc(sizeof(fftw_complex)*n);

	for (i = 0; i != n; i++) {
		in[i][0] = re[i];
		in[i][1] = im[i];
	}

	plan = fftw_plan_dft_1d(n, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(plan);

	for (i = 0; i != n; i++) {
		a = hypot(out[i][0], out[i][1]); // /n;
		a = a*a/n;
		res[i] = a;
	}
}


static void fft_real(int n, const float *re, const float *im, double *res)
{
	double *in;
	fftw_plan plan;
	int i;
	double a ;

	in = fftw_malloc(sizeof(double)*n);

	for (i = 0; i != n; i++) {
		a = hypot(re[i], im[i]);
		in[i] = a*a;
	}

	plan =  fftw_plan_r2r_1d(n, in, res, FFTW_REDFT10, FFTW_ESTIMATE);
	fftw_execute(plan);

	/* @@@ not sure at all about the scaling */
	for (i = 0; i != n; i++)
		res[i] = res[i]/sqrt(n);
}


static void do_fft(int skip, int dump, int low, int high, double threshold)
{
	float c[2];
	float *re = NULL, *im = NULL;
	double *res;
	int e = 0, n = 0;
	int i;
	double a;

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
		if (e <= n) {
			e = e ? e*2 : 10000;
			re = realloc(re, e*sizeof(float));
			im = realloc(im, e*sizeof(float));
			if (!re || !im) {
				perror("realloc");
				exit(1);
			}
		}
		re[n] = c[0];
		im[n] = c[1];
		n++;
	}

	if (skip >= n) {
		fprintf(stderr, "cannot skip %d of %d entries\n", skip, n);
		exit(1);
	}
	re += skip;
	im += skip;
	n -= skip;

	res = malloc(n*sizeof(double));
	if (!res) {
		perror("malloc");
		exit(1);
	}

	switch (alg) {
	case 0:
		fft_complex(n, re, im, res);
		break;
	case 1:
		fft_real(n, re, im, res);
		break;
	default:
		abort();
	}

	if (dump) {
		for (i = 0; i < n; i += 1)
			printf("%d %g\n", i, 10*log(res[i])/log(10));
	} else {
		double s = 0;
		double db;

		if (high >= n+skip) {
			fprintf(stderr, "end %d > number of samples %d\n",
			    high, n+skip);
			exit(1);
		}
		low = low*(double) n/(n+skip);
		high = high*(double) n/(n+skip);
		if (high < n)
			high++;
		if (low == high)
			low--;
		for (i = low; i != high; i++) {
			a = res[i];
			db = 10*log(a)/log(10);
			if (db >= threshold)
				s += a;
		}
		printf("%f\n", 10*log(s)/log(10));
	}
}


static void usage(const char *name)
{
	fprintf(stderr,
"usage: %s [-s skip] low high [threshold]\n"
"       %s [-s skip] -d\n\n"
"  threshold   only use frequency bins with at least this power, in - dB.\n"
"              E.g., a threshold value of 60 would be -60 dB. (default: %d\n"
"              dB)\n"
"  -d          dump frequency-domain \n"
"  -s skip     skip this number of samples from the beginning (default: 0)\n"
    , name, name, -DEFAULT_THRESHOLD);
	exit(1);
}


int main(int argc, char **argv)
{
	int dump = 0, skip = 0;
	int low, high;
	double threshold = DEFAULT_THRESHOLD;
	int c;

	while ((c = getopt(argc, argv, "a:ds:")) != EOF)
		switch (c) {
		case 'a':
			alg = atoi(optarg);
			break;
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
		if (!dump)
			usage(*argv);
		do_fft(skip, 1, 0, 0, 0);
		break;
	case 3:
		threshold = -atof(argv[optind+2]);
		/* fall through */
	case 2:
		if (dump)
			usage(*argv);
		low = atoi(argv[optind]);
		high = atoi(argv[optind+1]);
		if (low > high)
			usage(*argv);
		do_fft(skip, 0, low, high, threshold);
		break;
	default:
		usage(*argv);
	}

	return 0;
}
