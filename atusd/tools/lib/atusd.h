#ifndef ATUSD_H
#define	ATUSD_H

#include <stdint.h>


struct atusd_dsc;


struct atusd_dsc *atusd_open(void);
void atusd_close(struct atusd_dsc *dsc);
void atusd_cycle(struct atusd_dsc *dsc);
void atusd_reset(struct atusd_dsc *dsc);
void atusd_reg_write(struct atusd_dsc *dsc, uint8_t reg, uint8_t v);
uint8_t atusd_reg_read(struct atusd_dsc *dsc, uint8_t reg);

#endif /* ATUSD_H */
