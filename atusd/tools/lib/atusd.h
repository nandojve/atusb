struct atusd_dsc;


struct atusd_dsc *atusd_open(void);
void atusd_close(struct atusd_dsc *dsc);
void atusd_cycle(struct atusd_dsc *dsc);
void atusd_reset(struct atusd_dsc *dsc);
