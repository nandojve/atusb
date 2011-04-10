/*
 * lib/usbopen.c - Common USB device lookup and open code
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
#include <stdlib.h>
#include <stdio.h>
#include <usb.h>

#include "usbopen.h"


static uint16_t vendor = 0;
static uint16_t product = 0;


usb_dev_handle *open_usb(uint16_t default_vendor, uint16_t default_product)
{
	const struct usb_bus *bus;
	struct usb_device *dev;
	usb_dev_handle *handle;
#ifdef DO_FULL_USB_BUREAUCRACY
	int res;
#endif

	usb_init();
	usb_find_busses();
	usb_find_devices();

	for (bus = usb_get_busses(); bus; bus = bus->next)
		for (dev = bus->devices; dev; dev = dev->next) {
			if (dev->descriptor.idVendor !=
			    (vendor ? vendor : default_vendor))
				continue;
			if (dev->descriptor.idProduct !=
			    (product ? product : default_product))
				continue;
			handle = usb_open(dev);
#ifdef DO_FULL_USB_BUREAUCRACY
			if (!handle)
				return NULL;
			res = usb_set_configuration(handle, 1);
			if (res < 0) {
				fprintf(stderr, "usb_set_configuration: %d\n",
				    res);
				return NULL;
			}
			res = usb_claim_interface(handle, 0);
			if (res < 0) {
				fprintf(stderr, "usb_claim_interface: %d\n",
				    res);
				return NULL;
			}
			res = usb_set_altinterface(handle, 0);
			if (res < 0) {
				fprintf(stderr, "usb_set_altinterface: %d\n",
				    res);
				return NULL;
			}
#endif
			return handle;
		}
	return NULL;
}


static void bad_id(const char *id)
{
	fprintf(stderr, "\"%s\" is not a valid vendor:product ID\n", id);
	exit(1);
}


void parse_usb_id(const char *id)
{
	unsigned long tmp;
	char *end;

	tmp = strtoul(id, &end, 16);
	if (*end != ':')
		bad_id(id);
	if (tmp > 0xffff)
		bad_id(id);
	vendor = tmp;
	tmp = strtoul(end+1, &end, 16);
	if (*end)
		bad_id(id);
	if (tmp > 0xffff)
		bad_id(id);
	product = tmp;
}
