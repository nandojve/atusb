#ifdef HAVE_USB
/*
 * atusb-eui64/atusb-eui64.c - Read and write EUI64 address from/to EEPROM
 *
 * Written 2015, 2016 by Stefan Schmidt
 * Copyright 2015, 2016 Stefan Schmidt
 *
 * Based on atrf-id with following copyright:
 * Written 2010-2011, 2013 by Werner Almesberger
 * Copyright 2010-2011, 2013 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <usb.h>

#include "atusb/ep0.h"
#include "atrf.h"

#define	FROM_DEV	ATUSB_FROM_DEV(0)
#define	TO_DEV		ATUSB_TO_DEV(0)

#define	EUI64_LEN	8
#define	BUF_SIZE	256

static int get_eui64(usb_dev_handle *dev, void *data, int size)
{
	int res;

	res = usb_control_msg(dev, FROM_DEV, ATUSB_EUI64_READ, 0, 0, data,
			      size, 1000);
	if (res < 0)
		fprintf(stderr, "ATUSB_EUI64_READ: %s\n", usb_strerror());
	return res;
}

static int set_eui64(usb_dev_handle *dev, void *data, int size)
{
	int res;

	res = usb_control_msg(dev, TO_DEV, ATUSB_EUI64_WRITE, 0, 0, data, size,
			      1000);
	if (res < 0)
		fprintf(stderr, "ATUSB_EUI64_WRITE: %s\n", usb_strerror());
	return res;
}

static int get_id(usb_dev_handle *dev, void *data, int size)
{
	int res;

	res = usb_control_msg(dev, FROM_DEV, ATUSB_ID, 0, 0, data, size, 1000);
	if (res < 0)
		fprintf(stderr, "ATUSB_ID: %s\n", usb_strerror());
	return res;
}

static int get_protocol(usb_dev_handle *dev, uint8_t *major, uint8_t *minor,
			uint8_t *target)
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

static void show_usb_info(struct atrf_dsc *dsc, unsigned char *mac, int write)
{
	usb_dev_handle *dev;
	const struct usb_device *device;
	uint8_t major, minor, target;
	const char *mcu;
	char buf[BUF_SIZE+1];	/* +1 for terminating \0 */
	char serial[BUF_SIZE];
	int len, i;
	uint8_t eui64[EUI64_LEN];

	dev = atrf_usb_handle(dsc);
	if (!dev)
		return;
	device = usb_device(dev);

	printf("Found ATUSB device (%04x:%04x) ",
	    device->descriptor.idVendor, device->descriptor.idProduct);

	if (device->descriptor.iSerialNumber > 0)
		usb_get_string_simple(dev, device->descriptor.iSerialNumber,
				      serial, BUF_SIZE);

	printf("with serial number %s\n", serial);

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
	printf("Firmware version %u.%u on hw %u (%s) ", major, minor, target,
	       mcu);

	len = get_build(dev, buf, sizeof(buf)-1);
	if (len < 0)
		exit(1);
	buf[len] = 0;
	printf("build: %s\n", buf);

	if (target != HW_TYPE_110131) {
		printf("Firmware not able to set EUI64 on %s\n", mcu);
		exit(1);
	}

	if (major == 0 && minor < 3) {
		printf("Firmware to old. You need at least version 0.3\n");
		exit(1);
	}

	/* No new EUI64 is given, just read out */
	if (!write) {
		get_eui64(dev, eui64, EUI64_LEN);
		printf("Current EUI64 address from EEPROM: ");
		for (i = 0; i < EUI64_LEN; i++) {
			printf("%02X", eui64[EUI64_LEN - 1 - i]);
			if (i < EUI64_LEN -1)
				printf(":");
		}
		printf("\n");
	}
	/* We got a new EUI64 as argument, write it */
	if (write) {
		printf("Writing EUI64 address ");
		for (i = 0; i < EUI64_LEN; i++) {
			printf("%02x", mac[EUI64_LEN - 1 - i]);
			if (i < EUI64_LEN -1)
				printf(":");
		}
		printf(" to EEPROM\n");
		set_eui64(dev, mac, EUI64_LEN);
	}
}

/* Taken from wpan-tools/src/interface.c which is BSD licensed and written
 * by Alexander Aring
 */
static int extendedaddr_a2n(unsigned char *mac_addr, char *arg)
{
	int i;

	for (i = 0; i < EUI64_LEN ; i++) {
		int temp;
		char *cp = strchr(arg, ':');
		if (cp) {
			*cp = 0;
			cp++;
		}
		if (sscanf(arg, "%x", &temp) != 1)
			return -1;
		if (temp < 0 || temp > 255)
			return -1;

		mac_addr[EUI64_LEN - 1 - i] = temp;
		if (!cp)
			break;
		arg = cp;
	}
	if (i < EUI64_LEN - 1)
		return -3;

	return 0;
}

static void usage(const char *name)
{
	fprintf(stderr, "usage: %s [-a 00:11:22:33:44:55:66:77]\n", name);
	exit(1);
}

int main(int argc, char *const *argv)
{
	uint8_t mac[EUI64_LEN];
	struct atrf_dsc *dsc;
	int c;
	int write = 0;

	while ((c = getopt(argc, argv, "a:")) != EOF)
		switch (c) {
		case 'a':
			extendedaddr_a2n(mac, optarg);
			write = 1;
			break;
		default:
			usage(*argv);
		}

	dsc = atrf_open(NULL);
	if (!dsc)
		return 1;

	show_usb_info(dsc, mac, write);

	atrf_close(dsc);

	return 0;
}
#endif /* HAVE_USB */
