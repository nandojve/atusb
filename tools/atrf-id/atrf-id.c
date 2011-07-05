/*
 * atrf-id/atrf-id.c - Identify a ben-wpan AT86RF230 board
 *
 * Written 2010-2011 by Werner Almesberger
 * Copyright 2010-2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#ifdef HAVE_USB
#include <usb.h>
#endif

#include "at86rf230.h"
#include "atusb/ep0.h"
#include "atrf.h"


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


static void show_usb_info(struct atrf_dsc *dsc)
{
	usb_dev_handle *dev;
	const struct usb_device *device;
	uint8_t major, minor, target;
	const char *mcu;
	char buf[BUF_SIZE+1];	/* +1 for terminating \0 */
	int len;

	dev = atrf_usb_handle(dsc);
	if (!dev)
		return;
	device = usb_device(dev);

	printf("%04x:%04x ",
	    device->descriptor.idVendor, device->descriptor.idProduct);

	if (get_protocol(dev, &major, &minor, &target) < 0)
		exit(1);
	switch (target) {
	case HW_TYPE_100813:
	case HW_TYPE_101216:
		mcu = "C8051F326";
		break;
	case HW_TYPE_110131:
		mcu = "ATmega32U2";
		break;
	default:
		mcu = "???";
	}
	printf("protocol %u.%u hw %u (%s)\n", major, minor, target, mcu);

	len = get_build(dev, buf, sizeof(buf)-1);
	if (len < 0)
		exit(1);
	buf[len] = 0;
	printf("%10s%s\n", "", buf);
}


static void do_dump_debug(struct atrf_dsc *dsc)
{
	usb_dev_handle *dev;
	uint8_t buf[1024];
	int res;
	struct log {
		uint16_t n;
		uint8_t bmRequestType;
		uint8_t bRequest;
		uint16_t wValue;
		uint16_t wIndex;
		uint16_t wLength;
	} *p;

	dev = atrf_usb_handle(dsc);
	if (!dev)
		return;

	res = usb_control_msg(dev, FROM_DEV, ATUSB_READ_LOG, 0, 0,
	    (void *) buf, sizeof(buf), 1000);
	if (res < 0) {
		fprintf(stderr, "ATUSB_READ_LOG: %s\n", usb_strerror());
		return;
	}

	printf("Number\tbmRequestType\tbRequest\twValue\twIndex\twLength\n");
	for (p = (void *) buf; (uint8_t *) (p+1) <= buf+res; p++) {
		printf("%u\t", p->n);
		printf("0x%02x\t\t", p->bmRequestType);
		printf("0x%02x\t\t", p->bRequest);
		printf("0x%04x\t", p->wValue);
		printf("0x%04x\t", p->wIndex);
		printf("0x%04x\n", p->wLength);

	}
}


#else /* HAVE_USB */


static void show_usb_info(struct atrf_dsc *dsc)
{
}


static void do_dump_debug(struct atrf_dsc *dsc)
{
}


#endif /* !HAVE_USB */


static void show_info(struct atrf_dsc *dsc)
{
	uint8_t part, version, man_id_0, man_id_1;

	show_usb_info(dsc);

	printf("%10s", "");

	switch (atrf_identify(dsc)) {
	case atrf_unknown_chip:
		printf("???");
		break;
	case artf_at86rf230:
		printf("AT86RF230");
		break;
	case artf_at86rf231:
		printf("AT86RF231");
		break;
	default:
		abort();
	}

	part = atrf_reg_read(dsc, REG_PART_NUM);
	version = atrf_reg_read(dsc, REG_VERSION_NUM);
	man_id_0 = atrf_reg_read(dsc, REG_MAN_ID_0);
	man_id_1 = atrf_reg_read(dsc, REG_MAN_ID_1);
	printf(", part 0x%02x version %u manufacturer xxxx%02x%02x",
	    part, version, man_id_1, man_id_0);

	printf(" (%s)\n", man_id_1 == 0 && man_id_0 == 0x1f ? "Atmel" : "???");
}


static void usage(const char *name)
{
	fprintf(stderr,
"usage: %s [-d driver[:arg]] [-s [-s]]\n\n"
"  -d driver[:arg]  use the specified driver (default: %s)\n"
"  -D               dump atusb debug buffer\n"
"  -s               print only the local driver specification\n"
"  -s -s            print only the remote driver specification\n"
    , name, atrf_default_driver_name());
	exit(1);
}


int main(int argc, char *const *argv)
{
	const char *driver = NULL;
	struct atrf_dsc *dsc;
	int spec_only = 0;
	int dump_debug = 0;
	int c;

	while ((c = getopt(argc, argv, "d:Ds")) != EOF)
		switch (c) {
		case 'd':
			driver = optarg;
			break;
		case 'D':
			dump_debug = 1;
			break;
		case 's':
			spec_only++;
			break;
		default:
			usage(*argv);
		}
	if (argc != optind)
		usage(*argv);

	dsc = atrf_open(driver);
	if (!dsc)
		return 1;

	if (spec_only) {
		const char *spec = atrf_driver_spec(dsc, spec_only > 1);

		if (spec)
			printf("%s\n", spec);
		else {
			fprintf(stderr, "can't obtain specification\n");
			exit(1);
		}
	} else {
		show_info(dsc);
	}
	if (dump_debug)
		do_dump_debug(dsc);

	atrf_close(dsc);

	return 0;
}
