/*
 * atspi-id/atspi-id.c - Identify a ben-wpan AF86RF230 board
 *
 * Written 2010 by Werner Almesberger
 * Copyright 2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdlib.h>
#include <stdio.h>
#include <usb.h>

#include "atspi/ep0.h"
#include "atspi.h"


#define	FROM_DEV	ATSPI_FROM_DEV(0)

#define	BUF_SIZE	256



static int get_id(usb_dev_handle *dev, void *data, int size)
{
	int res;

	res = usb_control_msg(dev, FROM_DEV, ATSPI_ID, 0, 0, data, size, 1000);
	if (res < 0)
		fprintf(stderr, "ATSPI_ID: %s\n", usb_strerror());
	return res;
}


static int atspi_get_protocol(usb_dev_handle *dev,
    uint8_t *major, uint8_t *minor, uint8_t *target)
{
	uint8_t ids[3];

	if (get_id(dev, ids, 3) < 0)
		return -1;
	if (major)
		*major = ids[0];
	if (minor)
		*minor = ids[1];
	if (target)
		*target = ids[2];

	return 0;
}
 

static int atspi_get_build(usb_dev_handle *dev, char *buf, size_t size)
{
	int res;

	res = usb_control_msg(dev, FROM_DEV, ATSPI_BUILD, 0, 0, buf, size,
	    1000);
	if (res < 0)
		fprintf(stderr, "ATSPI_BUILD: %s\n", usb_strerror());
	return res;
}


static void show_info(usb_dev_handle *dev)
{
	const struct usb_device *device = usb_device(dev);
	uint8_t major, minor, target;
	char buf[BUF_SIZE+1];	/* +1 for terminating \0 */
	int len;

	printf("%04x:%04x ",
	    device->descriptor.idVendor, device->descriptor.idProduct);

	if (atspi_get_protocol(dev, &major, &minor, &target) < 0)
		exit(1);
	printf("protocol %u.%u hw %u\n", major, minor, target);

	len = atspi_get_build(dev, buf, sizeof(buf)-1);
	if (len < 0)
		exit(1);
	buf[len] = 0;
	printf("%10s%s\n", "", buf);
}


static void usage(const char *name)
{
	fprintf(stderr, "%s\n", name);
	exit(1);
}


int main(int argc, const char **argv)
{
	usb_dev_handle *dev;

	if (argc != 1)
		usage(*argv);
	dev = atspi_open();
	if (!dev)
		return 1;

	show_info(dev);

	return 0;
}
