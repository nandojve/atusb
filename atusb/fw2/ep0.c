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

#include <avr/io.h>
//#ifndef NULL
//#define NULL 0
//#endif

//#include "regs.h"
//#include "uart.h"
//#include "usb.h"

#include "freakusb.h"

#include "at86rf230.h"
#include "atusb/ep0.h"
//#include "version.h"
#include "board.h"
#include "spi.h"


#define	HW_TYPE		2	/* @@@ needs more work */
static const char *build_date = "today";
static unsigned build_number = 42;

#define debug(...)
#define error(...)


static const uint8_t id[] = { EP0ATUSB_MAJOR, EP0ATUSB_MINOR, HW_TYPE };
static uint8_t buf[MAX_PSDU+3]; /* command, PHDR, and LQI */
static uint8_t size;


static void do_buf_write(void *user)
{
	uint8_t i;

	spi_begin();
	for (i = 0; i != size; i++)
		spi_send(buf[i]);
	spi_end();
}


static void do_usb_send(const uint8_t *data, int len)
{
	int i;

	for (i = 0; i++ != len; data++) {
		usb_buf_write(EP_CTRL, *data);
		if (!(i % MAX_BUF_SZ))
			ep_write(EP_CTRL);
	}
	ep_write(EP_CTRL);
}


static void do_usb_recv(uint8_t *data, int len, void (*fn)(void *user),
    void *user)
{
	volatile usb_pcb_t *pcb = usb_pcb_get();
	uint8_t got;

	/* FreakUSB likes to do things this way. Looks dangerous ... */
	while (len) {
		for (got = pcb->fifo[EP_CTRL].len; got; got--) {
			*data++ = usb_buf_read(EP_CTRL);
			len--;
		}
	}
	pcb->flags &= ~(1 << SETUP_DATA_AVAIL);
	fn(user);
	ep_send_zlp(EP_CTRL);
}


#define	usb_send(ep, buf, len, arg1, arg2)	do_usb_send(buf, len)
#define	usb_recv(ep, buf, len, fn, user)	do_usb_recv(buf, len, fn, user)


#define	BUILD_OFFSET	7	/* '#' plus "65535" plus ' ' */


/* keep things as similar to the original as possible for now */
#define	setup_request	req_t
#define	setup		req
#define	bmRequestType	type
#define	bRequest	req
#define	wValue		val
#define	wLength		len
#define	wIndex		idx
#define	__reentrant


static int my_setup(struct setup_request *setup) __reentrant
{
	unsigned tmp;
	uint8_t i;

	switch (setup->bmRequestType | setup->bRequest << 8) {
	case ATUSB_FROM_DEV(ATUSB_ID):
		debug("ATUSB_ID\n");
		if (setup->wLength > 3)
			return 0;
		usb_send(&ep0, id, setup->wLength, NULL, NULL);
		return 1;
	case ATUSB_FROM_DEV(ATUSB_BUILD):
		debug("ATUSB_BUILD\n");
		tmp = build_number;
		for (i = BUILD_OFFSET-2; tmp; i--) {
			buf[i] = (tmp % 10)+'0';
			tmp /= 10;
		}
		buf[i] = '#';
		buf[BUILD_OFFSET-1] = ' ';
		for (size = 0; build_date[size]; size++)
			buf[BUILD_OFFSET+size] = build_date[size];
		size += BUILD_OFFSET-i;
		if (size > setup->wLength)
			return 0;
		usb_send(&ep0, buf+i, size, NULL, NULL);
		return 1;

#ifdef NOTYET
	case ATUSB_TO_DEV(ATUSB_RESET):
		debug("ATUSB_RESET\n");
		RSTSRC = SWRSF;
		while (1);
#endif

	case ATUSB_TO_DEV(ATUSB_RF_RESET):
		debug("ATUSB_RF_RESET\n");
		reset_rf();
		ep_send_zlp(EP_CTRL);
		return 1;

	case ATUSB_FROM_DEV(ATUSB_POLL_INT):
		debug("ATUSB_POLL_INT\n");
		if (setup->wLength < 1)
			return 0;
		*buf = 0;//IRQ_RF;
		usb_send(&ep0, buf, 1, NULL, NULL);
		return 1;

	case ATUSB_TO_DEV(ATUSB_REG_WRITE):
		debug("ATUSB_REG_WRITE\n");
		spi_begin();
		spi_send(AT86RF230_REG_WRITE | setup->wIndex);
		spi_send(setup->wValue);
		spi_end();
		ep_send_zlp(EP_CTRL);
		return 1;
	case ATUSB_FROM_DEV(ATUSB_REG_READ):
		debug("ATUSB_REG_READ\n");
		spi_begin();
		spi_send(AT86RF230_REG_READ | setup->wIndex);
		*buf = spi_recv();
		spi_end();
		usb_send(&ep0, buf, 1, NULL, NULL);
		return 1;

	case ATUSB_TO_DEV(ATUSB_BUF_WRITE):
		debug("ATUSB_BUF_WRITE\n");
		if (setup->wLength < 1)
			return 0;
		if (setup->wLength > MAX_PSDU)
			return 0;
		buf[0] = AT86RF230_BUF_WRITE;
		buf[1] = setup->wLength;
		size = setup->wLength+2;
		usb_recv(&ep0, buf+2, setup->wLength, do_buf_write, NULL);
		return 1;
	case ATUSB_FROM_DEV(ATUSB_BUF_READ):
		debug("ATUSB_BUF_READ\n");
		if (setup->wLength < 2)			/* PHR+LQI */
			return 0;
		if (setup->wLength > MAX_PSDU+2)	/* PHR+PSDU+LQI */
			return 0;
		spi_begin();
		spi_send(AT86RF230_BUF_READ);
		size = spi_recv();
		if (size >= setup->wLength)
			size = setup->wLength-1;
		for (i = 0; i != size+1; i++)
			buf[i] = spi_recv();
		spi_end();
		usb_send(&ep0, buf, size+1, NULL, NULL);
		return 1;

	case ATUSB_TO_DEV(ATUSB_SRAM_WRITE):
		debug("ATUSB_SRAM_WRITE\n");
		if (setup->wIndex > SRAM_SIZE)
			return 0;
		if (setup->wIndex+setup->wLength > SRAM_SIZE)
			return 0;
		buf[0] = AT86RF230_SRAM_WRITE;
		buf[1] = setup->wIndex;
		size = setup->wLength+2;
		usb_recv(&ep0, buf+2, setup->wLength, do_buf_write, NULL);
		return 1;
	case ATUSB_TO_DEV(ATUSB_SRAM_READ):
		debug("ATUSB_SRAM_READ\n");
		if (setup->wIndex > SRAM_SIZE)
			return 0;
		if (setup->wIndex+setup->wLength > SRAM_SIZE)
			return 0;
		spi_begin();
		spi_send(AT86RF230_SRAM_READ);
		spi_send(setup->wIndex);
		for (i = 0; i != size; i++)
			buf[i] = spi_recv();
		spi_end();
		usb_send(&ep0, buf, size, NULL, NULL);
		return 1;

	default:
		error("Unrecognized SETUP: 0x%02x 0x%02x ...\n",
		    setup->bmRequestType, setup->bRequest);
		return 0;
	}
}


static void class_init(void)
{
}


static void req_handler(req_t *req)
{
	if (!my_setup(req))
		ep_set_stall(EP_CTRL);
}


static void rx_handler(void)
{
}


void ep0_init(void)
{
        usb_reg_class_drvr(class_init, req_handler, rx_handler);
}
