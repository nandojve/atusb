/*
 * atspi/descr.c - USB descriptors
 *
 * Written 2008-2011 by Werner Almesberger
 * Copyright 2008-2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include "freakusb.h"
#include <avr/pgmspace.h>


#if 0
/* f32xbase/fw/common/usb.h */

#define USB_DT_DEVICE           1
#define USB_DT_CONFIG           2
#define USB_DT_STRING           3
#define USB_DT_INTERFACE        4
#define USB_DT_ENDPOINT         5

#endif

#define USB_CLASS_VENDOR_SPEC   0xff

#define USB_ATTR_BUS_POWERED    0x80

#define	USB_DT_DEVICE		DEV_DESCR
#define	USB_DT_CONFIG		CFG_DESCR
#define	USB_DT_INTERFACE	INTF_DESCR
#define	USB_DT_ENDPOINT		EP_DESCR

#define	EP0_SIZE		MAX_BUF_SZ
#define	EP1_SIZE		MAX_BUF_SZ
#define USB_VENDOR		0x20b7	/* Qi Hardware */
#define	USB_PRODUCT		0x1540	/* ben-wpan atusb */

#define LE(x) ((uint16_t) (x) & 0xff), ((uint16_t) (x) >> 8)

/*
 * Device descriptor
 */

uint8_t device_descriptor[18] PROGMEM = {
	18,			/* bLength */
	USB_DT_DEVICE,		/* bDescriptorType */
	LE(0x200),		/* bcdUSB */
	USB_CLASS_VENDOR_SPEC,	/* bDeviceClass */
	0x00,			/* bDeviceSubClass */
	0x00,			/* bDeviceProtocol */
	EP0_SIZE,		/* bMaxPacketSize */
	LE(USB_VENDOR),		/* idVendor */
	LE(USB_PRODUCT),	/* idProduct */
	LE(0x0001),		/* bcdDevice */
	0,			/* iManufacturer */
	0,			/* iProduct */
	0,			/* iSerialNumber */
	1			/* bNumConfigurations */
};


/*
 * Our configuration
 *
 * We're always bus-powered.
 */

uint8_t config_descriptor[] PROGMEM = {
	9,			/* bLength */
	USB_DT_CONFIG,		/* bDescriptorType */
#if 0
	LE(9+9+7+7),		/* wTotalLength */
#else
	LE(9+9),		/* wTotalLength */
#endif
	1,			/* bNumInterfaces */
	1,			/* bConfigurationValue (> 0 !) */
	0,			/* iConfiguration */
	USB_ATTR_BUS_POWERED,	/* bmAttributes */
	50/2,			/* bMaxPower (50 mA) */

	/* Interface #0 */

	9,			/* bLength */
	USB_DT_INTERFACE,	/* bDescriptorType */
	0,			/* bInterfaceNumber */
	0,			/* bAlternateSetting */
#if 0
	2,			/* bNumEndpoints */
#else
	0,
#endif
	USB_CLASS_VENDOR_SPEC,	/* bInterfaceClass */
	0,			/* bInterfaceSubClass */
	0,			/* bInterfaceProtocol */
	0,			/* iInterface */

#if 0
	/* EP OUT */

	7,			/* bLength */
	USB_DT_ENDPOINT,	/* bDescriptorType */
	0x01,			/* bEndPointAddress */
	0x02,			/* bmAttributes (bulk) */
	LE(EP1_SIZE),		/* wMaxPacketSize */
	0,			/* bInterval */

	/* EP IN */

	7,			/* bLength */
	USB_DT_ENDPOINT,	/* bDescriptorType */
	0x81,			/* bEndPointAddress */
	0x02,			/* bmAttributes (bulk) */
	LE(EP1_SIZE),		/* wMaxPacketSize */
	0,			/* bInterval */
#endif
};


#define	dev_desc	device_descriptor
#define	cfg_desc	config_descriptor

U8 *desc_dev_get()
{
    return dev_desc;
}
U8 desc_dev_get_len()
{
    return pgm_read_byte(dev_desc);
}
U8 *desc_cfg_get()
{
    return cfg_desc;
}
U8 desc_cfg_get_len()
{
    return pgm_read_byte(cfg_desc + 2);
}
U8 *desc_dev_qual_get()
{
    return NULL;
}
U8 desc_dev_qual_get_len()
{
    return 0;
}
U8 *desc_str_get(U8 index)
{
	return NULL;
}
U8 desc_str_get_len(U8 index)
{
	return 0;
}
