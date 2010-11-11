/*
 * atspi-id/atspi-id.c - Identify a ben-wpan AT86RF230 board
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

#ifdef HAVE_USB
#include <usb.h>
#endif

#include "at86rf230.h"
#include "atusb/ep0.h"
#include "atspi.h"


#ifdef HAVE_USB

#define	FROM_DEV	ATUSB_FROM_DEV(0)

#define	BUF_SIZE	256


static int get_id(usb_dev_handle *dev, void *data, int size)
{
	int res;

	res = usb_control_msg(dev, FROM_DEV, ATUSB_ID, 0, 0, data, size, 1000);
	if (res < 0)
		fprintf(stderr, "ATUSB_ID: %s\n", usb_strerror());
	return res;
}


static int get_protocol(usb_dev_handle *dev,
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
 

static int get_build(usb_dev_handle *dev, char *buf, size_t size)
{
	int res;

	res = usb_control_msg(dev, FROM_DEV, ATUSB_BUILD, 0, 0, buf, size,
	    1000);
	if (res < 0)
		fprintf(stderr, "ATUSB_BUILD: %s\n", usb_strerror());
	return res;
}


static void show_usb_info(struct atspi_dsc *dsc)
{
	usb_dev_handle *dev;
	const struct usb_device *device;
	uint8_t major, minor, target;
	char buf[BUF_SIZE+1];	/* +1 for terminating \0 */
	int len;

	dev = atspi_usb_handle(dsc);
	if (!dev)
		return;
	device = usb_device(dev);

	printf("%04x:%04x ",
	    device->descriptor.idVendor, device->descriptor.idProduct);

	if (get_protocol(dev, &major, &minor, &target) < 0)
		exit(1);
	printf("protocol %u.%u hw %u\n", major, minor, target);

	len = get_build(dev, buf, sizeof(buf)-1);
	if (len < 0)
		exit(1);
	buf[len] = 0;
	printf("%10s%s\n", "", buf);
}


#else /* HAVE_USB */


static void show_usb_info(struct atspi_dsc *dsc)
{
}


#endif /* !HAVE_USB */


static void show_info(struct atspi_dsc *dsc)
{
	uint8_t part, version, man_id_0, man_id_1;

	show_usb_info(dsc);

	part = atspi_reg_read(dsc, REG_PART_NUM);
	version = atspi_reg_read(dsc, REG_VERSION_NUM);
	man_id_0 = atspi_reg_read(dsc, REG_MAN_ID_0);
	man_id_1 = atspi_reg_read(dsc, REG_MAN_ID_1);
	printf("%10spart 0x%02x version %u manufacturer xxxx%02x%02x\n", "",
	    part, version, man_id_1, man_id_0);
}


static void usage(const char *name)
{
	fprintf(stderr, "usage: %s\n", name);
	exit(1);
}


int main(int argc, const char **argv)
{
	struct atspi_dsc *dsc;

	if (argc != 1)
		usage(*argv);
	dsc = atspi_open();
	if (!dsc)
		return 1;

	show_info(dsc);

	atspi_close(dsc);

	return 0;
}
