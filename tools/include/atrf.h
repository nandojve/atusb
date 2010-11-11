/*
 * include/atrf.h - ATRF access functions library
 *
 * Written 2010 by Werner Almesberger
 * Copyright 2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef ATRF_H
#define	ATRF_H

#include <stdint.h>


struct atrf_dsc;


void *atrf_usb_handle(struct atrf_dsc *dsc); /* hack for atspi-id */

struct atrf_dsc *atrf_open(void);
void atrf_close(struct atrf_dsc *dsc);

int atrf_error(struct atrf_dsc *dsc);
int atrf_clear_error(struct atrf_dsc *dsc);

void atrf_reset(struct atrf_dsc *dsc);
void atrf_reset_rf(struct atrf_dsc *dsc);
int atrf_test_mode(struct atrf_dsc *dsc);

void atrf_reg_write(struct atrf_dsc *dsc, uint8_t reg, uint8_t value);
uint8_t atrf_reg_read(struct atrf_dsc *dsc, uint8_t reg);

void atrf_buf_write(struct atrf_dsc *dsc, const void *buf, int size);
int atrf_buf_read(struct atrf_dsc *dsc, void *buf, int size);

int atrf_interrupt(struct atrf_dsc *dsc);

#endif /* !ATRF_H */
