/*
 * boot/dfu.c - DFU protocol engine
 *
 * Written 2008-2011 by Werner Almesberger
 * Copyright 2008-2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/*
 * http://www.usb.org/developers/devclass_docs/DFU_1.1.pdf
 */

/*
 * A few, erm, shortcuts:
 *
 * - we don't bother with the app* states since DFU is all this firmware does
 * - after DFU_DNLOAD, we just block until things are written, so we never
 *   enter dfuDNLOAD_SYNC or dfuDNBUSY
 * - no dfuMANIFEST_SYNC, dfuMANIFEST, or dfuMANIFEST_WAIT_RESET
 * - to keep our buffers small, we only accept EP0-sized blocks
 */


#include <stdint.h>

#include "usb.h"
#include "dfu.h"

#include "../board.h"


#ifndef NULL
#define NULL 0
#endif

#define debug(...)
#define error(...)


const uint8_t device_descriptor[] = {
	18,			/* bLength */
	USB_DT_DEVICE,		/* bDescriptorType */
	LE(0x100),		/* bcdUSB */
	USB_CLASS_PER_INTERFACE,/* bDeviceClass */
	0x00,			/* bDeviceSubClass (per interface) */
	0x00,			/* bDeviceProtocol (per interface) */
	EP0_SIZE,		/* bMaxPacketSize */
	LE(DFU_USB_VENDOR),	/* idVendor */
	LE(DFU_USB_PRODUCT),	/* idProduct */
	LE(0x0001),		/* bcdDevice */
	0,			/* iManufacturer */
	0,			/* iProduct */
	0,			/* iSerialNumber */
	1			/* bNumConfigurations */
};


const uint8_t config_descriptor[] = {
	9,			/* bLength */
	USB_DT_CONFIG,		/* bDescriptorType */
	LE(9+9),		/* wTotalLength */
	1,			/* bNumInterfaces */
	1,			/* bConfigurationValue (> 0 !) */
	0,			/* iConfiguration */
//	USB_ATTR_SELF_POWERED | USB_ATTR_BUS_POWERED,
	USB_ATTR_BUS_POWERED,	/* bmAttributes */
	15,			/* bMaxPower */

	/* Interface #0 */

	9,			/* bLength */
	USB_DT_INTERFACE,	/* bDescriptorType */
	0,			/* bInterfaceNumber */
	0,			/* bAlternateSetting */
	0,			/* bNumEndpoints */
	0xfe,			/* bInterfaceClass (application specific) */
	0x01,			/* bInterfaceSubClass (device fw upgrade) */
	0x02,			/* bInterfaceProtocol (DFU mode protocol) */
	0,			/* iInterface */
};


static const uint8_t functional_descriptor[] = {
	9,			/* bLength */
	DFU_DT_FUNCTIONAL,	/* bDescriptorType */
	0xf,			/* bmAttributes (claim omnipotence :-) */
	LE(0xffff),		/* wDetachTimeOut (we're very patient) */
	LE(EP0_SIZE),		/* wTransferSize */
	LE(0x101),		/* bcdDFUVersion */
};


/*
 * The worst-case activity would be flashing a one page and erasing another
 * one, would should take less than 10 ms. A 100 ms timeout ought to be plenty.
 */

struct dfu dfu = {
	OK,			/* bStatus */
	LE(100), 0,		/* bwPollTimeout, 100 ms */
	dfuIDLE,		/* bState */
	0,			/* iString */
};


static uint16_t next_block = 0;
static int did_download;


static uint8_t buf[EP0_SIZE];


static void block_write(void *user)
{
	uint16_t *size = user;

	flash_write(buf, *size);
}


static int block_receive(uint16_t length)
{
	static uint16_t size;

	if (!flash_can_write(length)) {
		dfu.state = dfuERROR;	
		dfu.status = errADDRESS;
		return 0;
	}
	if (length > EP0_SIZE) {
		dfu.state = dfuERROR;	
		dfu.status = errUNKNOWN;
		return 0;
	}
	size = length;
	usb_recv(&eps[0], buf, size, block_write, &size);
	return 1;
}


static int block_transmit(uint16_t length)
{
	uint16_t got;

	if (length > EP0_SIZE) {
		dfu.state = dfuERROR;	
		dfu.status = errUNKNOWN;
		return 1;
	}
	got = flash_read(buf, length);
	if (got < length) {
		length = got;
		dfu.state = dfuIDLE;
	}
	usb_send(&eps[0], buf, length, NULL, NULL);
	return 1;
}


static int my_setup(const struct setup_request *setup)
{
	int ok;

	switch (setup->bmRequestType | setup->bRequest << 8) {
	case DFU_TO_DEV(DFU_DETACH):
		debug("DFU_DETACH\n");
		/*
		 * The DFU spec says thay this is sent in protocol 1 only.
		 * However, dfu-util also sends it to get out of DFU mode,
		 * so we just don't make a fuss and ignore it.
		 */
		return 1;
	case DFU_TO_DEV(DFU_DNLOAD):
		debug("DFU_DNLOAD\n");
		if (dfu.state == dfuIDLE) {
			next_block = setup->wValue;
			flash_start();
		}
		else if (dfu.state != dfuDNLOAD_IDLE) {
			error("bad state\n");
			return 0;
		}
		if (dfu.state != dfuIDLE && setup->wValue == next_block-1) {
			debug("retransmisson\n");
			return 1;
		}
		if (setup->wValue != next_block) {
			debug("bad block (%d vs. %d)\n",
			    setup->wValue, next_block);
			dfu.state = dfuERROR;
			dfu.status = errUNKNOWN;
			return 1;
		}
		if (!setup->wLength) {
			debug("DONE\n");
			flash_end_write();
			dfu.state = dfuIDLE;
			did_download = 1;
			return 1;
		}
		ok = block_receive(setup->wLength);
		next_block++;
		dfu.state = dfuDNLOAD_IDLE;
		return ok;
	case DFU_FROM_DEV(DFU_UPLOAD):
		debug("DFU_UPLOAD\n");
		if (dfu.state == dfuIDLE) {
			next_block = setup->wValue;
			flash_start();
		}
		else if (dfu.state != dfuUPLOAD_IDLE)
			return 0;
		if (dfu.state != dfuIDLE && setup->wValue == next_block-1) {
			debug("retransmisson\n");
			/* @@@ try harder */
			dfu.state = dfuERROR;
			dfu.status = errUNKNOWN;
			return 1;
		}
		if (setup->wValue != next_block) {
			debug("bad block (%d vs. %d)\n",
			    setup->wValue, next_block);
			dfu.state = dfuERROR;
			dfu.status = errUNKNOWN;
			return 1;
		}
		ok = block_transmit(setup->wLength);
		next_block++;
		dfu.state = dfuUPLOAD_IDLE;
		return ok;
	case DFU_FROM_DEV(DFU_GETSTATUS):
		debug("DFU_GETSTATUS\n");
		usb_send(&eps[0], (uint8_t *) &dfu, sizeof(dfu), NULL, NULL);
		return 1;
	case DFU_TO_DEV(DFU_CLRSTATUS):
		debug("DFU_CLRSTATUS\n");
		dfu.state = dfuIDLE;
		dfu.status = OK;
		return 1;
	case DFU_FROM_DEV(DFU_GETSTATE):
		debug("DFU_GETSTATE\n");
		usb_send(&eps[0], &dfu.state, 1, NULL, NULL);
		return 1;
	case DFU_TO_DEV(DFU_ABORT):
		debug("DFU_ABORT\n");
		dfu.state = dfuIDLE;
		dfu.status = OK;
		return 1;
	default:
		error("DFU rt %x, rq%x ?\n",
		    setup->bmRequestType, setup->bRequest);
		return 0;
	}
}


static int my_descr(uint8_t type, uint8_t index, const uint8_t **reply,
    uint8_t *size)
{
	if (type != DFU_DT_FUNCTIONAL)
		return 0;
	*reply = functional_descriptor;
	*size = sizeof(functional_descriptor);
	return 1;
}


static void my_reset(void)
{
#if 0
	/* @@@ not nice -- think about where this should go */
	extern void run_payload(void);

	if (did_download)
		run_payload();
#endif
}


void dfu_init(void)
{
	user_setup = my_setup;
	user_get_descriptor = my_descr;
	user_reset = my_reset;
}
