#include <stdio.h>
#include <math.h>


#define N	100


int main(int argc, char **argv)
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
				return 1;
			}
		}
		sum += hypot(c[0], c[1]);
		if (n++ % N)
			continue;
		printf("%f\n", sum/N);
		sum = 0;
	}
	return 0;
}
