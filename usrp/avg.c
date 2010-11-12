#include <stdlib.h>
#include <stdio.h>
#include <math.h>


#define SAMPLES_DEFAULT	100


static void average(int samples)
{
	float c[2];
	int n = 0;
	float sum = 0;
	size_t s;


	while (1) {
		s = fread(c, sizeof(c), 1, stdin);
		if (!s) {
			if (!ferror(stdin))
				break;
			if (s < 0) {
				perror("read");
				exit(1);
			}
		}
		sum += hypot(c[0], c[1]);
		if (n++ % samples)
			continue;
		printf("%f\n", sum/samples);
		sum = 0;
	}
}


static void usage(const char *name)
{
	fprintf(stderr,
"usage: %s [samples]\n\n"
"  samples  samples to average over (default: %d)\n"
    , name, SAMPLES_DEFAULT);
	exit(1);
}


int main(int argc, char **argv)
{
	int n = SAMPLES_DEFAULT;

	switch (argc) {
	case 1:
		break;
	case 2:
		n = atoi(argv[1]);
		if (n <= 0)
			usage(*argv);
		break;
	default:
		usage(*argv);
	}

	average(n);

	return 0;
}
