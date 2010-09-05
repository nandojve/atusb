/*
 * include/atspi.h - ATSPI access functions library
 *
 * Written 2010 by Werner Almesberger
 * Copyright 2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef ATSPI_H
#define	ATSPI_H

#include <stdint.h>


struct atspi_dsc;


int atspi_error(void);
int atspi_clear_error(void);

struct atspi_dsc *atspi_open(void);
void atspi_close(struct atspi_dsc *dsc);

void atspi_reset(struct atspi_dsc *dsc);
void atspi_reset_rf(struct atspi_dsc *dsc);

void atspi_reg_write(struct atspi_dsc *dsc, uint8_t reg, uint8_t value);
uint8_t atspi_reg_read(struct atspi_dsc *dsc, uint8_t reg);

void atspi_buf_write(struct atspi_dsc *dsc, const void *buf, int size);
int atspi_buf_read(struct atspi_dsc *dsc, void *buf, int size);

#endif /* !ATSPI_H */
