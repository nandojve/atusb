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
#include <usb.h>


int atspi_error(void);
int atspi_clear_error(void);

usb_dev_handle *atspi_open(void);
void atspi_close(usb_dev_handle *dev);

void atspi_reset(usb_dev_handle *dev);
void atspi_reset_rf(usb_dev_handle *dev);

void atspi_reg_write(usb_dev_handle *dev, uint8_t reg, uint8_t value);
uint8_t atspi_reg_read(usb_dev_handle *dev, uint8_t reg);

void atspi_buf_write(usb_dev_handle *dev, const void *buf, int size);
int atspi_buf_read(usb_dev_handle *dev, void *buf, int size);

#endif /* !ATSPI_H */
