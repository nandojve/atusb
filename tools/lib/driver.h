/*
 * lib/driver.h - ATRF driver API
 *
 * Written 2010-2011 by Werner Almesberger
 * Copyright 2010-2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef DRIVER_H
#define	DRIVER_H

#include <stdint.h>


struct atrf_driver {
	const char *name;
	void *(*open)(const char *arg);
	void (*close)(void *dsc);
	int (*error)(void *dsc);
	int (*clear_error)(void *dsc);
	void (*reset)(void *dsc);
	void (*reset_rf)(void *dsc);
	void (*test_mode)(void *dsc);
	void (*slp_tr)(void *dsc, int on);
	int (*set_clkm)(void *dsc, int mhz);
	void (*reg_write)(void *dsc, uint8_t reg, uint8_t value);
	uint8_t (*reg_read)(void *dsc, uint8_t reg);
	void (*buf_write)(void *dsc, const void *buf, int size);
	int (*buf_read)(void *dsc, void *buf, int size);
	int (*interrupt)(void *dsc);
};


extern struct atrf_driver atusb_driver;
extern struct atrf_driver atben_driver;
extern struct atrf_driver atnet_driver;


int atrf_set_clkm_generic(
    void (*reg_write)(void *dsc, uint8_t reg, uint8_t value),
    void *handle, int mhz);

void *atben_regs(void *dsc); /* hack for atrf-xtal */
void *atusb_dev_handle(void *dsc); /* hack for atrf-id */

#endif /* !DRIVER_H */
