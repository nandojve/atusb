#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>


#define PERC	0.9
#define	SKIP	1000000


static int comp(const void *_a, const void *_b)
{
	float a = *(const float *) _a;
	float b = *(const float *) _b;

	return a < b ? -1 : a > b;
}


int main(int argc, char **argv)
{
	float max = 0;
	float c[2], a;
	float *rec = NULL;
	int e = 0, n = 0, skip = SKIP;

	while (1) {
		size_t s;

		s = fread(c, sizeof(c), 1, stdin);
		if (!s) {
			if (!ferror(stdin))
				break;
			if (s < 0) {
				perror("read");
				return 1;
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
	qsort(rec, n, sizeof(float), comp);
	printf("%f %f\n", max, rec[(int) (PERC*n)]);
#if 0
int i;

	for (i = 0; i < n; i += 1000)
		printf("%f %f\n", (double) i/n, rec[i]);
#endif
	return 0;
}
