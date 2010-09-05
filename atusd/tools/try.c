#include <stdio.h>
#include <unistd.h>

#include "lib/atusd.h"


int main(void)
{
	struct atusd_dsc *dsc;
	char tmp;

	dsc = atusd_open();
	read(1, &tmp, 1);
fprintf(stderr, "cycling\n");
	atusd_cycle(dsc);
	read(1, &tmp, 1);
	atusd_close(dsc);

	return 0;
}
