/*
 * atspi/ep0.c - EP0 extension protocol
 *
 * Written 2008-2010 by Werner Almesberger
 * Copyright 2008-2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdint.h>

#ifndef NULL
#define NULL 0
#endif

#include "regs.h"
//#include "uart.h"
#include "usb.h"
#include "atspi/ep0.h"
#include "at86rf230.h"
#include "version.h"


#define debug(...)
#define error(...)


static const uint8_t id[] = { EP0ATSPI_MAJOR, EP0ATSPI_MINOR, HW_TYPE };
static __xdata uint8_t buf[MAX_PSDU+3]; /* command, PHDR, and LQ */
static uint8_t size;


static void spi_send(uint8_t v)
{
	uint8_t mask;

	for (mask = 0x80; mask; mask >>= 1) {
		MOSI = !!(v & mask);
		SCLK = 1;
		SCLK = 0;
	}
}


static uint8_t spi_recv(void)
{
	uint8_t res = 0;
	uint8_t i;

	for (i = 0; i != 8; i++) {
		res = (res << 1) | MISO;
		SCLK = 1;
		SCLK = 0;
	}
	return res;
}


static void do_buf_write(void *user)
{
	uint8_t i;

	user; /* suppress warning */
	nSS = 0;
	spi_send(AT86RF230_BUF_WRITE);
	for (i = 0; i != size; i++)
		spi_send(buf[i]);
	nSS = 1;
}


static __bit my_setup(struct setup_request *setup) __reentrant
{
	uint8_t i;

	switch (setup->bmRequestType | setup->bRequest << 8) {
	case ATSPI_FROM_DEV(ATSPI_ID):
		debug("ATSPI_ID\n");
		if (setup->wLength > 3)
			return 0;
		usb_send(&ep0, id, setup->wLength, NULL, NULL);
		return 1;
	case ATSPI_FROM_DEV(ATSPI_BUILD_NUMBER):
		debug("ATSPI_BUILD_NUMBER\n");
		if (setup->wLength > 2)
			return 0;
		usb_send(&ep0, (void *) &build_number, setup->wLength,
		    NULL, NULL);
		return 1;
	case ATSPI_FROM_DEV(ATSPI_BUILD_DATE):
		debug("ATSPI_BUILD_DATE\n");
		for (size = 0; build_date[size]; size++);
		if (size > EP1_SIZE)
			return 0;
		if (size > setup->wLength)
			return 0;
		usb_send(&ep0, build_date, size, NULL, NULL);
		return 1;
	case ATSPI_TO_DEV(ATSPI_RESET):
		debug("ATSPI_RESET\n");
		RSTSRC = SWRSF;
		while (1);

	case ATSPI_TO_DEV(ATSPI_RF_RESET):
		debug("ATSPI_RF_RESET\n");
		nRST_RF = 0;
		/* 11.4.12 min 625 ns */
		nRST_RF = 1;
		return 1;

	case ATSPI_TO_DEV(ATSPI_REG_WRITE):
		debug("ATSPI_REG_WRITE\n");
		nSS = 0;
		spi_send(AT86RF230_REG_WRITE | setup->wIndex);
		spi_send(setup->wValue);
		nSS = 1;
		return 1;
	case ATSPI_FROM_DEV(ATSPI_REG_READ):
		debug("ATSPI_REG_READ\n");
		nSS = 0;
		spi_send(AT86RF230_REG_READ | setup->wIndex);
		*buf = spi_recv();
		nSS = 1;
		usb_send(&ep0, buf, 1, NULL, NULL);
		return 1;

	case ATSPI_TO_DEV(ATSPI_BUF_WRITE):
		debug("ATSPI_BUF_WRITE\n");
		if (setup->wLength < 1)
			return 0;
		if (setup->wLength > MAX_PSDU+1)	/* PHR+PSDU */
			return 0;
		buf[0] = AT86RF230_BUF_WRITE;
		size = setup->wLength+1;
		usb_recv(&ep0, buf+1, setup->wLength, do_buf_write, NULL);
		return 1;
	case ATSPI_TO_DEV(ATSPI_BUF_READ):
		debug("ATSPI_BUF_READ\n");
		if (setup->wLength < 2)			/* PHR+LQ */
			return 0;
		if (setup->wLength > MAX_PSDU+2)	/* PHR+PSDU+LQ */
			return 0;
		nSS = 0;
		spi_send(AT86RF230_BUF_READ);
		size = *buf = spi_recv();
		if (size+2 > setup->wLength)
			size = setup->wLength-2;
		for (i = 0; i != size+1; i++)
			buf[i+1] = spi_recv();
		nSS = 1;
		usb_send(&ep0, buf, size+1, NULL, NULL);
		return 1;

	case ATSPI_TO_DEV(ATSPI_SRAM_WRITE):
		debug("ATSPI_SRAM_WRITE\n");
		if (setup->wIndex > SRAM_SIZE)
			return 0;
		if (setup->wIndex+setup->wLength > SRAM_SIZE)
			return 0;
		buf[0] = AT86RF230_SRAM_WRITE;
		buf[1] = setup->wIndex;
		size = setup->wLength+2;
		usb_recv(&ep0, buf+2, setup->wLength, do_buf_write, NULL);
		return 1;
	case ATSPI_TO_DEV(ATSPI_SRAM_READ):
		debug("ATSPI_SRAM_READ\n");
		if (setup->wIndex > SRAM_SIZE)
			return 0;
		if (setup->wIndex+setup->wLength > SRAM_SIZE)
			return 0;
		nSS = 0;
		spi_send(AT86RF230_SRAM_READ);
		spi_send(setup->wIndex);
		for (i = 0; i != size; i++)
			buf[i] = spi_recv();
		nSS = 1;
		usb_send(&ep0, buf, size, NULL, NULL);
		return 1;

	default:
		error("Unrecognized SETUP: 0x%02x 0x%02x ...\n",
		    setup->bmRequestType, setup->bRequest);
		return 0;
	}
}


void ep0_init(void)
{
	user_setup = my_setup;
}
