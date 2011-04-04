/*
 * atrf-xtal/atben.c - ATBEN-specific low-level driver
 *
 * Written 2011 by Werner Almesberger
 * Copyright 2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/*
 * WARNING: this program does very nasty things to the Ben and it doesn't
 * like company. In particular, it resents:
 *
 * - the MMC driver - disable it with
 *   echo jz4740-mmc.0 >/sys/bus/platform/drivers/jz4740-mmc/unbind
 * - the AT86RF230/1 kernel driver - use a kernel that doesn't have it
 * - anything that accesses the screen - kill GUI, X server, etc.
 * - the screen blanker - either disable it or make sure the screen stays
 *   dark, e.g., with
 *   echo 1 >/sys/devices/platform/jz4740-fb/graphics/fb0/blank
 * - probably a fair number of other daemons and things as well - best to
 *   kill them all.
 */


#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#include "at86rf230.h"
#include "atrf.h"

#include "atrf-xtal.h"


#define	MAX_COUNT	(1000*1000)


static volatile void *base;
static volatile uint32_t *clkgr;
static uint32_t old_clkgr;


void atben_setup(struct atrf_dsc *dsc, int size, int trim)
{
	static uint8_t buf[127];

	atrf_reset_rf(dsc);
	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_TRX_OFF);

	atrf_reg_write(dsc, REG_XOSC_CTRL,
	    (XTAL_MODE_INT << XTAL_MODE_SHIFT) | trim);

	/* minimum TX power, maximize delays, disable CRC */
	switch (atrf_identify(dsc)) {
	case artf_at86rf230:
		atrf_reg_write(dsc, REG_PHY_TX_PWR, 0xf);
		break;
	case artf_at86rf231:
		atrf_reg_write(dsc, REG_PHY_TX_PWR, 0xff);
		atrf_reg_write(dsc, REG_TRX_CTRL_1, 0);
		break;
	default:
		abort();
	}

	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_PLL_ON);
	usleep(200);	/* nominally 180 us worst-case */

	atrf_buf_write(dsc, buf, size);

	//atrf_reg_write(dsc, REG_IRQ_MASK, IRQ_TRX_END);
	atrf_reg_write(dsc, REG_IRQ_MASK, 0xff);

	mlockall(MCL_CURRENT);

	struct atrf_dsc {
		void *driver;
		void *handle;
		int chip_id;
	};
	struct atben_dsc {
		int fd;
		void *mem;
	};

	base = ((struct atben_dsc *) ((struct atrf_dsc *) dsc)->handle)->mem;
	clkgr = base+0x20;

	old_clkgr = *clkgr;
	*clkgr = old_clkgr | 1 << 10;
}


unsigned atben_sample(struct atrf_dsc *dsc)
{
	unsigned i = MAX_COUNT;

	(void) atrf_reg_read(dsc, REG_IRQ_STATUS);
#if 0
	while (i) {
		if (atrf_interrupt(dsc))
			break;
		i--;
	}
#else
	volatile uint32_t *pdpin = base+0x10300;
	volatile uint32_t *pddats = base+0x10314;
	volatile uint32_t *pddatc = base+0x10318;
	volatile uint32_t *icmr = base+0x1004;
	volatile uint32_t *icmsr = base+0x1008;
	volatile uint32_t *icmcr = base+0x100c;
#if 0
	atrf_slp_tr(dsc, 1);
	while (i) {
		if (*pdpin & 0x1000)
			break;
		i--;
	}
	atrf_slp_tr(dsc, 0);
#endif
	uint32_t old_icmr = *icmr;
	*icmsr = 0xffffffff;

	*pddats = 1 << 9;
	*pddatc = 1 << 9;
	do i--;
	while (!(*pdpin & 0x1000));

	*icmcr = ~old_icmr;
#endif
	return MAX_COUNT-i;
}


void atben_cleanup(struct atrf_dsc *dsc)
{
	*clkgr = old_clkgr;
}
