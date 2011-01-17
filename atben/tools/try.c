#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include "at86rf230.h"
#include "lib/atusd.h"


int main(void)
{
	struct atusd_dsc *dsc;
	uint8_t part, version, man_id_0, man_id_1;

	dsc = atusd_open();

	atusd_cycle(dsc);
//	atusd_reset(dsc);

        part = atusd_reg_read(dsc, REG_PART_NUM);
        version = atusd_reg_read(dsc, REG_VERSION_NUM);
        man_id_0 = atusd_reg_read(dsc, REG_MAN_ID_0);
        man_id_1 = atusd_reg_read(dsc, REG_MAN_ID_1);
        printf("part 0x%02x version %u manufacturer xxxx%02x%02x\n",
            part, version, man_id_1, man_id_0);

	atusd_close(dsc);

	return 0;
}
