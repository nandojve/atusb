/*
 * lib/atusd.c - ATSPI access functions library (uSD version)
 *
 * Written 2010 by Werner Almesberger
 * Copyright 2010 Werner Almesberger
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
#include <fcntl.h>
#include <sys/mman.h>

#include "at86rf230.h"
#include "driver.h"


enum {
	VDD_OFF	= 1 << 6,	/* VDD disable, PD06 */
	MxSx	= 1 << 8,	/* CMD, PD08 */
	CLK	= 1 << 9,	/* CLK, PD09 */
	SCLK	= 1 << 10,	/* DAT0, PD10 */
	SLP_TR	= 1 << 11,	/* DAT1, PD11 */
	IRQ	= 1 << 12,	/* DAT2, PD12 */
	nSEL	= 1 << 13,	/* DAT3/CD, PD13 */
};


#define SOC_BASE	0x10000000

#define	REG(n)	(*(volatile uint32_t *) (dsc->mem+(n)))

#define CGU(n)	REG(0x00000+(n))
#define GPIO(n)	REG(0x10000+(n))
#define MSC(n)	REG(0x21000+(n))

#define	PDPIN	GPIO(0x300)	/* port D pin level */
#define PDDATS	GPIO(0x314)	/* port D data set */
#define PDDATC	GPIO(0x318)	/* port D data clear */
#define PDFUNS	GPIO(0x344)	/* port D function set */
#define PDFUNC	GPIO(0x348)	/* port D function clear */
#define PDDIRS	GPIO(0x364)	/* port D direction set */
#define PDDIRC	GPIO(0x368)	/* port D direction clear */

#define MSC_STRPCL	MSC(0x00)	/* Start/stop MMC/SD clock */
#define MSC_CLKRT	MSC(0x08)	/* MSC Clock Rate */

#define CLKGR		CGU(0x0020)	/* Clock Gate */
#define MSCCDR		CGU(0x0068)	/* MSC device clock divider */


#define	PAGE_SIZE	4096


struct atusd_dsc {
	int fd;
	void *mem;
};


/* ----- Reset functions --------------------------------------------------- */


static void atusd_cycle(struct atusd_dsc *dsc)
{
	/* stop the MMC bus clock */
	MSC_STRPCL = 1;

	/* drive all outputs low (including the MMC bus clock) */
	PDDATC = MxSx | CLK | SCLK | SLP_TR | nSEL;

	/* make the MMC bus clock a regular output */
	PDFUNC = CLK;

	/* cut the power */
	PDDATS = VDD_OFF;

	/* Power drains within about 20 ms. Wait 100 ms to be sure. */
	usleep(100*1000);

	/* drive nSS high */
	PDDATS = nSEL;

	/* supply power */
	PDDATS = VDD_OFF;

	/* return the bus clock output to the MMC controller */
	PDFUNS = CLK;

	/* start MMC clock output */
	MSC_STRPCL = 2;

	/*
	 * Give power time to stabilize and the chip time to reset.
	 * Experiments show that even usleep(0) is long enough.
	 */
	usleep(10*1000);
}


#if 0 /* we probably won't need this anymore */
static void atusd_reset(struct atusd_dsc *dsc)
{
	/* activate reset */
	PDDATS = SLP_TR;
	PDDATC = nSEL;

	/*
	 * Data sheet says 625 ns, programmer's guide says 6 us. Whom do we
	 * trust ?
	 */
	usleep(6);

	/* release reset */
	PDDATS = nSEL;
	PDDATC = SLP_TR;
}
#endif


/* ----- Low-level SPI operations ------------------------------------------ */


static void spi_begin(struct atusd_dsc *dsc)
{
	PDDATC = nSEL;
}


static void spi_end(struct atusd_dsc *dsc)
{
	PDDATS = nSEL;
}


static void spi_data_in(struct atusd_dsc *dsc)
{
	PDDIRC = MxSx;
}


static void spi_data_out(struct atusd_dsc *dsc)
{
	PDDIRS = MxSx;
}



/*
 * Send a sequence of bytes but leave the clock high on the last bit, so that
 * we can turn around the data line for reads.
 */

static void spi_send_partial(struct atusd_dsc *dsc, uint8_t v)
{
	uint8_t mask;

	for (mask = 0x80; mask; mask >>= 1) {
		PDDATC = SCLK;
		if (v & mask)
			PDDATS = MxSx;
		else
			PDDATC = MxSx;
		PDDATS = SCLK;
	}
}


static uint8_t spi_recv(struct atusd_dsc *dsc)
{
	uint8_t res = 0;
        uint8_t mask;

	for (mask = 0x80; mask; mask >>= 1) {
		PDDATC = SCLK;
		if (PDPIN & MxSx)
			res |= mask;
		PDDATS = SCLK;
	}
	PDDATC = SCLK;
        return res;
}


static void spi_finish(struct atusd_dsc *dsc)
{
	PDDATC = SCLK;
}


static void spi_send(struct atusd_dsc *dsc, uint8_t v)
{
	spi_send_partial(dsc, v);
	spi_finish(dsc);
}


/* ----- Driver operations ------------------------------------------------- */


static void *atusd_open(void)
{
	struct atusd_dsc *dsc;

	dsc = malloc(sizeof(*dsc));
	if (!dsc) {
		perror("malloc");
		exit(1);
	}

	dsc->fd = open("/dev/mem", O_RDWR);
	if (dsc->fd < 0) {
		perror("/dev/mem");
		exit(1);
	}
	dsc->mem = mmap(NULL, PAGE_SIZE*3*16, PROT_READ | PROT_WRITE,
	    MAP_SHARED, dsc->fd, SOC_BASE);
        if (dsc->mem == MAP_FAILED) {
                perror("mmap");
                exit(1);
        }

	/* set the output levels */
	PDDATS = nSEL | VDD_OFF;
	PDDATC = SCLK | SLP_TR;

	/* take the GPIOs away from the MMC controller */
	PDFUNC = MxSx | SCLK | SLP_TR | IRQ | nSEL;
	PDFUNS = CLK;

	/* set the pin directions */
	PDDIRC = IRQ;
	PDDIRS = MxSx | CLK | SCLK | SLP_TR | nSEL;

	/* enable power */
	PDDATC = VDD_OFF;

	/* set the MSC clock to 316 MHz / 21 = 16 MHz */
	MSCCDR = 20;
	/*
	 * Enable the MSC clock. We need to do this before accessing any
	 * registers of the MSC block !
	 */
	CLKGR &= ~(1 << 7);
	/* bus clock = MSC clock / 1 */
	MSC_CLKRT = 0;
	/* start MMC clock output */
	MSC_STRPCL = 2;

	return dsc;
}


static void atusd_close(void *arg)
{
	struct atusd_dsc *dsc = arg;

	/* stop the MMC bus clock */
	MSC_STRPCL = 1;

	/* cut the power */
	PDDATS = VDD_OFF;

	/* make all MMC pins inputs */
	PDDIRC = MxSx | CLK | SCLK | SLP_TR | IRQ | nSEL;
}


static void atusd_reset_rf(void *handle)
{
	struct atusd_dsc *dsc = handle;

	atusd_cycle(dsc);
}


static void atusd_reg_write(void *handle, uint8_t reg, uint8_t v)
{
	struct atusd_dsc *dsc = handle;

	spi_begin(dsc);
	spi_send(dsc, AT86RF230_REG_WRITE | reg);
	spi_send(dsc, v);
	spi_end(dsc);
}


static uint8_t atusd_reg_read(void *handle, uint8_t reg)
{
	struct atusd_dsc *dsc = handle;
	uint8_t res;

	spi_begin(dsc);
	spi_send_partial(dsc, AT86RF230_REG_READ| reg);
	spi_data_in(dsc);
	res = spi_recv(dsc);
	spi_finish(dsc);
	spi_data_out(dsc);
	spi_end(dsc);
	return res;
}


/* ----- Driver interface -------------------------------------------------- */


struct atspi_driver atusd_driver = {
	.name		= "uSD",
	.open		= atusd_open,
	.close		= atusd_close,
	.reset		= NULL,
	.reset_rf	= atusd_reset_rf,
	.reg_write	= atusd_reg_write,
	.reg_read	= atusd_reg_read,
#if 0
	.buf_write	= atusd_buf_write,
	.buf_read	= atusd_buf_read,
#endif
};
