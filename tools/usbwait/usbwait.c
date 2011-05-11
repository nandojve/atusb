/*
 * usbwait/usbwait.c - Wait for a USB device to appear
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
#include <sys/time.h>

#include "usbopen.h"


#define	DEFAULT_POLL_S	0.1


static useconds_t interval_us = DEFAULT_POLL_S*1000000;
static unsigned long timeout = 0;
static int need_removal = 0;


static void wait_for_usb(void)
{
	struct timeval to, now;
	usb_dev_handle *dev;

	gettimeofday(&to, NULL);
	to.tv_sec += timeout;

	while (1) {
		usb_rescan();
		dev = open_usb(0, 0);
		if (dev) {
			if (!need_removal)
				return;
			usb_close(dev);
		} else {
			need_removal = 0;
		}
		if (timeout) {
			gettimeofday(&now, NULL);
			if (now.tv_sec > to.tv_sec)
				break;
			if (now.tv_sec == to.tv_sec &&
			    now.tv_usec > to.tv_usec)
				break;
		}
		if (usleep(interval_us) < 0) {
			perror("usleep");
			exit(1);
		}
	}

	fprintf(stderr, "timeout\n");
	exit(1);
}


static void usage(const char *name)
{
	fprintf(stderr,
"usage: %s [-i poll_s] [-p path] [-r] [-t timeout_s] [vendor]:[product]\n\n"
"  -i poll_s     poll interval in seconds (default: %g s)\n"
"  -p path       USB device path\n"
"  -r            wait for device removal first\n"
"  -t timeout_s  timeout in seconds (default: infinite)\n"
  , name, DEFAULT_POLL_S);
	exit(1);
}


int main(int argc, char **argv)
{
	char *end;
	int c;

	while ((c = getopt(argc, argv, "i:p:rt:")) != EOF)
		switch (c) {
		case 'i':
			interval_us = strtod(optarg, &end)*1000000;
			if (*end || interval_us < 0)
				usage(*argv);
			break;
		case 'p':
			restrict_usb_path(optarg);
			break;
		case 'r':
			need_removal = 1;
			break;
		case 't':
			timeout = strtoul(optarg, &end, 0);
			if (*end || !timeout)
				usage(*argv);
			break;
		default:
			usage(*argv);
		}

	if (argc != optind+1)
		usage(*argv);

	parse_usb_id(argv[optind]);
	wait_for_usb();

	return 0;
}
