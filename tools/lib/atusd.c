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
	VDD_OFF	= 1 << 2,	/* VDD disable, PD02 */
	MOSI	= 1 << 8,	/* CMD, PD08 */
	CLK	= 1 << 9,	/* CLK, PD09 */
	MISO	= 1 << 10,	/* DAT0, PD10 */
	SCLK	= 1 << 11,	/* DAT1, PD11 */
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


static void wait_for_power(void)
{
	/*
	 * Give power time to stabilize and the chip time to reset.
	 * Power takes about 2 ms to ramp up. We wait 10 ms to be sure.
	 */
	usleep(10*1000);
}


static void atusd_cycle(struct atusd_dsc *dsc)
{
	/* stop the MMC bus clock */
	MSC_STRPCL = 1;

	/* drive all outputs low (including the MMC bus clock) */
	PDDATC = MOSI | CLK | SCLK | nSEL;

	/* make the MMC bus clock a regular output */
	PDFUNC = CLK;

	/* cut the power */
	PDDATS = VDD_OFF;

	/* Power drains within about 20 ms. Wait 100 ms to be sure. */
	usleep(100*1000);

	/* drive MOSI and nSS high */
	PDDATS = MOSI | nSEL;

	/* precharge the capacitors to avoid current surge */
	wait_for_power();

	/* return the bus clock output to the MMC controller */
	PDFUNS = CLK;

	/* start MMC clock output */
	MSC_STRPCL = 2;

	/* supply power */
	PDDATC = VDD_OFF;

	wait_for_power();
}


/* ----- Low-level SPI operations ------------------------------------------ */


static void spi_begin(struct atusd_dsc *dsc)
{
	PDDATC = nSEL;
}


static void spi_end(struct atusd_dsc *dsc)
{
	PDDATS = nSEL;
}


static void spi_send(struct atusd_dsc *dsc, uint8_t v)
{
	uint8_t mask;

	for (mask = 0x80; mask; mask >>= 1) {
		if (v & mask)
			PDDATS = MOSI;
		else
			PDDATC = MOSI;
		PDDATS = SCLK;
		PDDATC = SCLK;
	}
}


static uint8_t spi_recv(struct atusd_dsc *dsc)
{
	uint8_t res = 0;
        uint8_t mask;

	for (mask = 0x80; mask; mask >>= 1) {
		if (PDPIN & MISO)
			res |= mask;
		PDDATS = SCLK;
		PDDATC = SCLK;
	}
        return res;
}


/* ----- Driver operations ------------------------------------------------- */


static void atusd_reset_rf(void *handle)
{
	struct atusd_dsc *dsc = handle;

	atusd_cycle(dsc);
	wait_for_power();
}


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
	PDDATC = SCLK;

	/* take the GPIOs away from the MMC controller */
	PDFUNC = MOSI | MISO | SCLK | IRQ | nSEL;
	PDFUNS = CLK;

	/* set the pin directions */
	PDDIRC = MISO | IRQ;
	PDDIRS = MOSI | CLK | SCLK | nSEL;

	/* let capacitors precharge */
	wait_for_power();

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

	wait_for_power();
	atusd_reset_rf(dsc);

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
	PDDIRC = MOSI | MISO | CLK | SCLK | IRQ | nSEL;
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
	spi_send(dsc, AT86RF230_REG_READ | reg);
	res = spi_recv(dsc);
	spi_end(dsc);
	return res;
}


static void atusd_buf_write(void *handle, const void *buf, int size)
{
	struct atusd_dsc *dsc = handle;

	spi_begin(dsc);
	spi_send(dsc, AT86RF230_BUF_WRITE);
	spi_send(dsc, size);
	while (size--)
		spi_send(dsc, *(uint8_t *) buf++);
	spi_end(dsc);
}


static int atusd_buf_read(void *handle, void *buf, int size)
{
	struct atusd_dsc *dsc = handle;
	uint8_t len, i;

	spi_begin(dsc);
	spi_send(dsc, AT86RF230_BUF_READ);
	len = spi_recv(dsc);
	len++; /* LQI */
	if (len > size)
		len = size;
	for (i = 0; i != len; i++)
		*(uint8_t *) buf++ = spi_recv(dsc);
	spi_end(dsc);
	return len;
}


/* ----- RF interrupt ------------------------------------------------------ */


static int atusd_interrupt(void *handle)
{
        struct atusd_dsc *dsc = handle;

	return !!(PDPIN & IRQ);
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
	.buf_write	= atusd_buf_write,
	.buf_read	= atusd_buf_read,
	.interrupt	= atusd_interrupt,
};
