/*
 * lib/misctxrx.c - Miscellaenous transceiver helper functions
 *
 * Written 2010-2011 by Werner Almesberger
 * Copyright 2010-2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>

#include "at86rf230.h"
#include "atrf.h"
#include "misctxrx.h"


/* ----- Interrupts -------------------------------------------------------- */


static volatile int run = 1;


static void die(int sig)
{
	run = 0;
}


uint8_t wait_for_interrupt(struct atrf_dsc *dsc, uint8_t wait_for,
    uint8_t ignore, int sleep_us, int timeout)
{
	uint8_t irq = 0, show;
	void (*old_sig)(int);

	run = 1;
	old_sig = signal(SIGINT, die);
	while (run) {
		while (run && !atrf_interrupt(dsc)) {
			usleep(sleep_us);
			if (timeout && !--timeout) {
				irq = 0;
				goto out;
			}
		}
		irq = atrf_reg_read(dsc, REG_IRQ_STATUS);
		if (atrf_error(dsc))
			exit(1);
		if (!irq)
			continue;
		show = irq & ~ignore;
		if (!show) {
			if (irq & wait_for)
				break;
			continue;
		}
		fprintf(stderr, "IRQ (0x%02x):", irq);
		if (irq & IRQ_PLL_LOCK)
			fprintf(stderr, " PLL_LOCK");
		if (irq & IRQ_PLL_UNLOCK)
			fprintf(stderr, " PLL_UNLOCK");
		if (irq & IRQ_RX_START)
			fprintf(stderr, " RX_START");
		if (irq & IRQ_TRX_END)
			fprintf(stderr, " TRX_END");
		if (irq & IRQ_CCA_ED_DONE)
			fprintf(stderr, " CCA_ED_DONE");
		if (irq & IRQ_AMI)
			fprintf(stderr, " AMI");
		if (irq & IRQ_TRX_UR)
			fprintf(stderr, " TRX_UR");
		if (irq & IRQ_BAT_LOW)
			fprintf(stderr, " BAT_LOW");
		fprintf(stderr, "\n");
		if (irq & wait_for)
			break;
	}
out:
	signal(SIGINT, old_sig);
	if (!run)
		raise(SIGINT);
	return irq;
}


/* ----- Transmit power ---------------------------------------------------- */


static const double tx_pwr_230[] = {
	 3.0,	 2.6,	 2.1,	 1.6,
	 1.1,	 0.5,	-0.2,	-1.2,
	-2.2,	-3.2,	-4.2,	-5.2,
	-7.2,	-9.2,	-12.2,	-17.2
};


static const double tx_pwr_231[] = {
	 3.0,	 2.8,	 2.3,	 1.8,
	 1.3,	 0.7,	 0.0,	-1,
	-2,	-3,	-4,	-5,
	-7,	-9,	-12,	-17
};


#define	POWER_TABLE_SIZE	(sizeof(tx_pwr_230)/sizeof(*tx_pwr_230))


void set_power_step(struct atrf_dsc *dsc, int power, int crc)
{
	uint8_t tmp;

	switch (atrf_identify(dsc)) {
	case artf_at86rf230:
		atrf_reg_write(dsc, REG_PHY_TX_PWR,
		    (crc ? TX_AUTO_CRC_ON_230 : 0) | power);
		break;
	case artf_at86rf231:
		tmp = atrf_reg_read(dsc, REG_PHY_TX_PWR);
		tmp = (tmp & ~TX_PWR_MASK) | power;
		atrf_reg_write(dsc, REG_PHY_TX_PWR, tmp);
		atrf_reg_write(dsc, REG_TRX_CTRL_1,
		    crc ? TX_AUTO_CRC_ON : 0);
		break;
	default:
		abort();
	}
}


static const double *tx_power_table(struct atrf_dsc *dsc)
{
	switch (atrf_identify(dsc)) {
	case artf_at86rf230:
		return tx_pwr_230;
	case artf_at86rf231:
		return tx_pwr_231;
		break;
	default:
		abort();
	}
}


int tx_power_dBm2step(struct atrf_dsc *dsc, double power)
{
	const double *tx_pwr = tx_power_table(dsc);
	int n;

	for (n = 0; n != POWER_TABLE_SIZE-1; n++)
		if (tx_pwr[n] <= power)
			break;
	return n;
}


double tx_power_step2dBm(struct atrf_dsc *dsc, int step)
{
	const double *tx_pwr = tx_power_table(dsc);

	if (step < 0 || step >= POWER_TABLE_SIZE)
		abort();
	return tx_pwr[step];
}


void set_power_dBm(struct atrf_dsc *dsc, double power, int crc)
{
	int step;
	double got;

	step = tx_power_dBm2step(dsc, power);
	got = tx_power_step2dBm(dsc, step);

	if (fabs(got-power) > 0.01)
		fprintf(stderr, "TX power %.1f dBm\n", got);

	set_power_step(dsc, step, crc);
}
