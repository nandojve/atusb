/*
 * u/usb.c - USB hardware setup and standard device requests
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
 * Known issues:
 * - no suspend/resume
 * - EP0-sized packets cause an (otherwise harmless) SUEND at the end of the
 *   packet
 * - #ifdef hell
 */

/*
 * This code follows the register read/write sequences from the examples in
 * SiLabs/MCU/Examples/C8051F326_7/USB_Interrupt/Firmware/F326_USB_Main.c and
 * SiLabs/MCU/Examples/C8051F326_7/USB_Interrupt/Firmware/F326_USB_ISR.c
 *
 * More resources:
 * http://www.beyondlogic.org/usbnutshell/usb1.htm
 */

#include <stdint.h>

#define F_CPU   8000000UL
#include <util/delay.h>

#include <avr/io.h>
#include "usb.h"
#include "../board.h"


#ifndef NULL
#define NULL 0
#endif

#define NUM_EPS	1

#if 1
extern void panic(void);
#define BUG_ON(cond)	do { if (cond) panic(); } while (0)
#else
#define BUG_ON(cond)
#endif

struct ep_descr eps[5];

int (*user_setup)(struct setup_request *setup);
int (*user_get_descriptor)(uint8_t type, uint8_t index,
    const uint8_t * const *reply, uint8_t *size);
void (*user_reset)(void);


static uint8_t addr;


void usb_io(struct ep_descr *ep, enum ep_state state, uint8_t *buf,
    uint8_t size, void (*callback)(void *user), void *user)
{
	BUG_ON(ep->state);
	ep->state = state;
	ep->buf = buf;
	ep->end = buf+size;
	ep->callback = callback;
	ep->user = user;
}


static uint16_t usb_read_word(void)
{
	uint8_t low;

	low = UEDATX;
	return low | UEDATX << 8;
}


static int get_descriptor(uint8_t type, uint8_t index, uint16_t length)
{
	const uint8_t *reply;
	uint8_t size;

	switch (type) {
	case USB_DT_DEVICE:
		reply = device_descriptor;
		size = reply[0];
		break;
	case USB_DT_CONFIG:
		if (index)
			return 0;
		reply = config_descriptor;
		size = reply[2];
		break;
	default:
		if (!user_get_descriptor)
			return 0;
		if (!user_get_descriptor(type, index, &reply, &size))
			return 0;
	}
	if (length < size)
		size = length;
	usb_send(&eps[0], reply, size, NULL, NULL);
	return 1;
}


static void enable_addr(void *user)
{
	while (!(UEINTX & (1 << TXINI)));
	UDADDR = addr | 1 << ADDEN;
}


/*
 * Process a SETUP packet. Hardware ensures that length is 8 bytes.
 */


static int handle_setup(void)
{
	struct setup_request setup;

	BUG_ON(UEBCLX < 8);

	setup.bmRequestType = UEDATX;
	setup.bRequest = UEDATX;
	setup.wValue = usb_read_word();
	setup.wIndex = usb_read_word();
	setup.wLength = usb_read_word();

//	UEINTX &= ~(1 << RXSTPI);

	switch (setup.bmRequestType | setup.bRequest << 8) {

	/*
	 * Device request
	 *
	 * See http://www.beyondlogic.org/usbnutshell/usb6.htm
	 */

	case FROM_DEVICE(GET_STATUS):
		if (setup.wLength != 2)
			return 0;
		usb_send(&eps[0], "\000", 2, NULL, NULL);
		break;
	case TO_DEVICE(CLEAR_FEATURE):
		break;
	case TO_DEVICE(SET_FEATURE):
		return 0;
	case TO_DEVICE(SET_ADDRESS):
		addr = setup.wValue;
		UDADDR = addr;
		usb_send(&eps[0], NULL, 0, enable_addr, NULL);
		break;
	case FROM_DEVICE(GET_DESCRIPTOR):
		if (!get_descriptor(setup.wValue >> 8, setup.wValue,
		    setup.wLength))
			return 0;
		break;
	case TO_DEVICE(SET_DESCRIPTOR):
		return 0;
	case FROM_DEVICE(GET_CONFIGURATION):
		usb_send(&eps[0], "", 1, NULL, NULL);
		break;
	case TO_DEVICE(SET_CONFIGURATION):
		if (setup.wValue != config_descriptor[5])
			return 0;
		break;

	/*
	 * Interface request
	 */

	case FROM_INTERFACE(GET_STATUS):
		return 0;
	case TO_INTERFACE(CLEAR_FEATURE):
		return 0;
	case TO_INTERFACE(SET_FEATURE):
		return 0;
	case FROM_INTERFACE(GET_INTERFACE):
		return 0;
	case TO_INTERFACE(SET_INTERFACE):
		{
			const uint8_t *interface_descriptor =
			    config_descriptor+9;

			if (setup.wIndex != interface_descriptor[2] ||
			    setup.wValue != interface_descriptor[3])
				return 0;
		}
		break;

	/*
	 * Endpoint request
	 */

	case FROM_ENDPOINT(GET_STATUS):
		return 0;
	case TO_ENDPOINT(CLEAR_FEATURE):
		return 0;
	case TO_ENDPOINT(SET_FEATURE):
		return 0;
	case FROM_ENDPOINT(SYNCH_FRAME):
		return 0;

	default:
		if (!user_setup)
			return 0;
		if (!user_setup(&setup))
			return 0;
	}

	if (!(setup.bmRequestType & 0x80) && eps[0].state == EP_IDLE)
		usb_send(&eps[0], NULL, 0, NULL, NULL);
	return 1;
}


static int ep_rx(struct ep_descr *ep)
{
	uint8_t size;

	size = UEBCLX;
	if (size > ep->end-ep->buf)
		return 0;
	while (size--)
		*ep->buf++ = UEDATX;
	if (ep->buf == ep->end) {
		ep->state = EP_IDLE;
		if (ep->callback)
			ep->callback(ep->user);
		if (ep == &eps[0])
			usb_send(ep, NULL, 0, NULL, NULL);
	}
	return 1;
}


static void ep_tx(struct ep_descr *ep)
{
	uint8_t size = ep->end-ep->buf;
	uint8_t left;

	if (size > ep->size)
		size = ep->size;
	for (left = size; left; left--)
		UEDATX = *ep->buf++;
	if (size == ep->size)
		return;
	ep->state = EP_IDLE;
}


static void handle_ep(int n)
{
	struct ep_descr *ep = eps+n;

	UENUM = n;
	if (UEINTX & (1 << RXSTPI)) {
		/* @@@ EP_RX. EP_TX: cancel */
		if (!handle_setup())
			goto stall;
		UEINTX &= ~(1 << RXSTPI);
	}
	if (UEINTX & (1 << RXOUTI)) {
		/* @@ EP_TX: cancel */
		if (ep->state != EP_RX)
			goto stall;
		if (!ep_rx(ep))
			goto stall;
//		UEINTX &= ~(1 << RXOUTI);
		UEINTX &= ~(1 << RXOUTI | 1 << FIFOCON);
	}
	if (UEINTX & (1 << STALLEDI)) {
		ep->state = EP_IDLE;
		UEINTX &= ~(1 << STALLEDI);
	}
	if (UEINTX & (1 << TXINI)) {
		/* @@ EP_RX: cancel */
		if (ep->state == EP_TX) {
			ep_tx(ep);
			UEINTX &= ~(1 << TXINI);
			if (ep->state == EP_IDLE && ep->callback)
				ep->callback(ep->user);
		}
	}
	return;

stall:
	UEINTX &= ~(1 << RXSTPI | 1 << RXOUTI | 1 << STALLEDI);
	ep->state = EP_IDLE;
	UECONX |= 1 << STALLRQ;
}


void usb_poll(void)
{
	uint8_t flags, i;

	flags = UEINT;
	for (i = 0; i != NUM_EPS; i++)
		if (1 || flags & (1 << i))
			handle_ep(i);
	/* @@@ USB bus reset */
}


static void ep_init(void)
{
	UENUM = 0;
	UECONX = (1 << RSTDT) | (1 << EPEN);	/* enable */
	UECFG0X = 0;	/* control, direction is ignored */
	UECFG1X = 3 << EPSIZE0;	/* 64 bytes */
	UECFG1X |= 1 << ALLOC;

	while (!(UESTA0X & (1 << CFGOK)));

	eps[0].state = EP_IDLE;
	eps[0].size = 64;
}


void usb_init(void)
{
	USBCON |= 1 << FRZCLK;		/* freeze the clock */

	/* enable the PLL and wait for it to lock */
	PLLCSR &= ~(1 << PLLP2 | 1 << PLLP1 | 1 << PLLP0);
	PLLCSR |= 1 << PLLE;
	while (!(PLLCSR & (1 << PLOCK)));

	USBCON &= ~(1 << USBE);		/* reset the controller */
	USBCON |= 1 << USBE;

	USBCON &= ~(1 << FRZCLK);	/* thaw the clock */

	UDCON &= ~(1 << DETACH);	/* attach the pull-up */
	UDCON |= 1 << RSTCPU;		/* reset CPU on bus reset */

	ep_init();
}
