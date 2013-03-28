/*
 * atrf-xtal/atben.c - ATBEN-specific driver and evaluation
 *
 * Written 2011, 2013 by Werner Almesberger
 * Copyright 2011, 2013 Werner Almesberger
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
 * - the AT86RF230/1 kernel driver - either use a kernel that doesn't have
 *   it or disable it with
 *   echo spi2.0 >/sys/bus/spi/drivers/at86rf230/unbind
 * - anything that accesses the screen - kill GUI, X server, etc.
 * - the screen blanker - either disable it or make sure the screen stays
 *   dark, e.g., with
 *   echo 1 >/sys/devices/platform/jz4740-fb/graphics/fb0/blank
 * - probably a fair number of other daemons and things as well - best to
 *   kill them all.
 */


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <sys/mman.h>

#include "at86rf230.h"
#include "atrf.h"

#include "atrf-xtal.h"


#define	MAX_COUNT	(1000*1000)


/* ----- RF setup ---------------------------------------------------------- */


static void rf_setup(struct atrf_dsc *dsc, int size, int trim)
{
	static uint8_t buf[127];

	atrf_reset_rf(dsc);
	atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_TRX_OFF);

	atrf_reg_write(dsc, REG_XOSC_CTRL,
	    (XTAL_MODE_INT << XTAL_MODE_SHIFT) | trim);

	/* minimum TX power, maximize delays, disable CRC */
	switch (atrf_identify(dsc)) {
	case atrf_at86rf230:
		atrf_reg_write(dsc, REG_PHY_TX_PWR, 0xf);
		break;
	case atrf_at86rf231:
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
}


/* ----- Ben hardware ------------------------------------------------------ */


static volatile uint32_t *icmr, *icmsr, *icmcr;
static uint32_t old_icmr;

static volatile uint32_t *clkgr;
static uint32_t old_clkgr;

static volatile uint32_t *pdpin, *pddats, *pddatc;


static void disable_interrupts(void)
{
	/*
	 * @@@ Race condition alert ! If we get interrupted/preempted between
	 * reading ICMR and masking all interrupts, and the code that runs
	 * between these two operations changes ICMR, then we may set an
	 * incorrect mask when restoring interrupts, which may hang the system.
	 */

	old_icmr = *icmr;
	*icmsr = 0xffffffff;
}


static void enable_interrupts(void)
{
	*icmcr = ~old_icmr;
}


/*
 * @@@ Disabling the LCD clock will halng operations that depend on the LCD
 * subsystem to advance. This includes the screen saver.
 */

static void disable_lcd(void)
{
	old_clkgr = *clkgr;
	*clkgr = old_clkgr | 1 << 10;
}


static void enable_lcd(void)
{
	*clkgr = old_clkgr;
}


static void ben_setup(struct atrf_dsc *dsc)
{
	volatile void *base = atrf_ben_regs(dsc);

	icmr = base+0x1004;
	icmsr = base+0x1008;
	icmcr = base+0x100c;

	clkgr = base+0x20;

	pdpin = base+0x10300;
	pddats = base+0x10314;
	pddatc = base+0x10318;

	/*
	 * Ironically, switching the LCD clock on and off many times only
	 * increases the risk of a hang. Therefore, we leave it stopped during
	 * all the measurements and only enable it again at the end.
	 */
	disable_lcd();
}


/* ----- Low-level interface ----------------------------------------------- */


void atben_setup(struct atrf_dsc *dsc, int size, int trim)
{
	rf_setup(dsc, size, trim);
	mlockall(MCL_CURRENT);
	ben_setup(dsc);
}


unsigned atben_sample(struct atrf_dsc *dsc)
{
	unsigned i = MAX_COUNT;

	(void) atrf_reg_read(dsc, REG_IRQ_STATUS);

	disable_interrupts();

#if 0
	/*
	 * This is a high-level view of what the code should do. It has rather
	 * high overhead, though, so we optimize it below.
	 */

	atrf_slp_tr(dsc, 1, 1);
	while (i) {
		if (atrf_interrupt(dsc))
			break;
		i--;
	}
#else
	/*
	 * We hit registers directly. We also don't enforce the upper limit,
	 * to squeeze out a few more cycles and gain a finer resolution.
	 */

	/* pulse SLP_TR */
	*pddats = 1 << 9;
	*pddatc = 1 << 9;

	/* count the time until an interrupt arrives */
	do i--;
	while (!(*pdpin & 0x1000));

#endif

	enable_interrupts();

	return MAX_COUNT-i;
}


void atben_cleanup(struct atrf_dsc *dsc)
{
	enable_lcd();
}


/* ----- Acquisition and evaluation ---------------------------------------- */


static int cmp(const void *a, const void *b)
{
	return *(unsigned *) a-*(unsigned *) b;
}



static double eval(unsigned *res, int rep)
{
	double sum = 0;
	int n = 0;
	int i;

	qsort(res, rep, sizeof(*res), cmp);
	if (rep < 8)
		return res[rep >> 1];
	for (i = rep/8; i != rep-rep/8; i++) {
		sum += res[i];
		n++;
	}
	return (double) sum/n;
}


void do_atben(struct atrf_dsc *dsc, int size, int trim, int rep,
    int dump_raw, double base, double ppm)
{
	unsigned *res;
	double avg, rel;
	int i;

	res = malloc(rep*sizeof(*res));
	if (!res) {
		perror("malloc");
		exit(1);
	}

	atben_setup(dsc, size, trim);

	for (i = 0; i != rep; i++)
		res[i] = atben_sample(dsc);

	atben_cleanup(dsc);

	atrf_close(dsc);

	if (dump_raw) {
		for (i = 0; i != rep; i++)
			printf("%u\n", res[i]);
		exit(0);
	}

	avg = eval(res, rep);
	if (!base)
		printf("%f\n", avg);
	else {
		rel = (avg/base-1)*1000000;
		printf("%+f ppm", rel);
		if (ppm && fabs(rel) > ppm) {
			printf(" (outside bounds)\n");
			exit(1);
		}
		putchar('\n');
	}
}
