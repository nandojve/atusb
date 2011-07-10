/*
 * usbperf/usbperf.c - Measure the rate of control transfers a device can do
 *
 * Written 2011 by Werner Almesberger
 * Copyright 2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/time.h>

#include <usb.h>

#include "usbopen.h"


#define	SYSFS_USB_BASE	"/sys/bus/usb/devices"


static void speed(usb_dev_handle *dev)
{
	struct usb_device *device = usb_device(dev);
	struct usb_bus *bus = device->bus;
	DIR *dir;
	const struct dirent *de;
	char buf[1000]; /* @@@ plenty :) */
	FILE *file;
	int res, devnum;
	float speed;

	dir = opendir(SYSFS_USB_BASE);
	if (!dir) {
		perror(SYSFS_USB_BASE);
		exit(1);
	}

	while ((de = readdir(dir))) {
		if (atoi(de->d_name) != atoi(bus->dirname))
			continue;

		sprintf(buf, SYSFS_USB_BASE "/%s/devnum", de->d_name);
		file = fopen(buf, "r");
		if (!file)
			continue;
		res = fscanf(file, "%d", &devnum);
		fclose(file);

		if (res != 1 || devnum != atoi(device->filename))
			continue;

		sprintf(buf, SYSFS_USB_BASE "/%s/speed", de->d_name);
		file = fopen(buf, "r");
		if (!file)
			continue;
		res = fscanf(file, "%f", &speed);
		fclose(file);

		printf("%g Mbps\n", speed);
	}
	closedir(dir);
}


static void rounds(usb_dev_handle *dev, int n)
{
	struct timeval t0, t1, t2;
	double s = 0, d;
	char buf;
	int i, res;

	gettimeofday(&t0, NULL);
	for (i = 0; i != n; i++) {
		gettimeofday(&t1, NULL);
		/* GET_CONFIGURATION */
		res = usb_control_msg(dev, USB_ENDPOINT_IN | USB_RECIP_DEVICE,
		    USB_REQ_GET_CONFIGURATION, 0, 0, &buf, 1, 1000);
	        if (res < 0)
			fprintf(stderr, "usb_control_msg returns %d\n", res);
		gettimeofday(&t2, NULL);
		s += (t2.tv_sec-t1.tv_sec)+(t2.tv_usec-t1.tv_usec)*1e-6;

	}
	d = (t2.tv_sec-t0.tv_sec)+(t2.tv_usec-t0.tv_usec)*1e-6;
	printf("Overall: %.3f ms/req\n", d/n*1000.0);
	printf("Each: %.3f ms/req\n", s/n*1000.0);
}


static void usage(const char *name)
{
	fprintf(stderr,
"usage: %s [vendor]:[product] [rounds]\n"
  , name);
	exit(1);
}


int main(int argc, char **argv)
{
	usb_dev_handle *dev;
	int n = 1000;

	switch (argc) {
	case 2:
		break;
	case 3:
		n = atoi(argv[2]);
		if (n <= 0)
			usage(*argv);
		break;
	default:
		usage(*argv);
	}

	parse_usb_id(argv[1]);
	dev = open_usb(0, 0);
	if (!dev)
		return 1;

	speed(dev);
	rounds(dev, n);

	return 0;
}
