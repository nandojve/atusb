/*
 * cntr/cntr.c - CNTR control tool
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
#include <signal.h>
#include <usb.h>
#include <sys/time.h>

#include "f32xbase/usb.h"
#include "cntr/ep0.h"
#include "cntr/usb-ids.h"


#define	FROM_DEV	CNTR_FROM_DEV(0)
#define	TO_DEV		CNTR_TO_DEV(0)

#define	DEFAULT_CLOCK_DEV_S	0.1	/* 100 ms, typ. NTP over WLAN dev. */
#define	BUF_SIZE		256


static int debug = 0;
static int verbose = 0;


/* ----- CRC, shared with firmware ----------------------------------------- */


/* crc32() */

#include "cntr/crc32.c"


/* ----- reset ------------------------------------------------------------- */


static void reset_cntr(usb_dev_handle *dev)
{
	int res;

	res = usb_control_msg(dev, TO_DEV, CNTR_RESET, 0, 0, NULL, 0, 1000);
	if (res < 0) {
		fprintf(stderr, "CNTR_RESET: %d\n", res);
		exit(1);
	}
}


/* ----- identify ---------------------------------------------------------- */


static void identify_cntr(usb_dev_handle *dev)
{
	const struct usb_device *device = usb_device(dev);
	uint8_t ids[3];
	char buf[BUF_SIZE+1];	/* +1 for terminating \0 */
	int res;

	printf("%04x:%04x ",
	    device->descriptor.idVendor, device->descriptor.idProduct);

	res = usb_control_msg(dev, FROM_DEV, CNTR_ID, 0, 0,
	    (char *) ids, sizeof(ids), 1000);
	if (res < 0) {
		fprintf(stderr, "CNTR_ID: %s\n", usb_strerror());
		exit(1);
	}

	printf("protocol %u.%u hw %u\n", ids[0], ids[1], ids[2]);

	res = usb_control_msg(dev, FROM_DEV, CNTR_BUILD, 0, 0,
	    buf, sizeof(buf), 1000);
	if (res < 0) {
		fprintf(stderr, "CNTR_BUILD: %s\n", usb_strerror());
		exit(1);
	}
	buf[res] = 0;
	printf("%10s%s\n", "", buf);
}


/* ----- measurements ------------------------------------------------------ */


struct sample {
	double t0, t1;
	uint64_t cntr;
};


static unsigned packets = 0, crc_errors = 0, inv_errors = 0;
static volatile int stop = 0;


static void set_stop(int sig)
{
	stop = 1;
}


static int get_sample(usb_dev_handle *dev, struct sample *s)
{
	static uint32_t last = 0, high = 0;
	struct timeval t0, t1;
	int res, bad;
	uint8_t buf[12];
	uint32_t cntr, inv, crc, expect;

	gettimeofday(&t0, NULL);
	res = usb_control_msg(dev, FROM_DEV, CNTR_READ, 0, 0,
	    (char *) buf, sizeof(buf), 1000);
	gettimeofday(&t1, NULL);
	if (res < 0) {
		fprintf(stderr, "CNTR_READ: %s\n", usb_strerror());
		exit(1);
	}
	packets++;
	cntr = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
	crc = buf[4] | (buf[5] << 8) | (buf[6] << 16) | (buf[7] << 24);
	inv = buf[8] | (buf[9] << 8) | (buf[10] << 16) | (buf[11] << 24);
	expect = crc32(cntr, ~0);
	bad = 0;
	if (crc != expect) {
		if (verbose)
			fprintf(stderr, "\nCRC error (count 0x%08x->0x%08x "
			    "CRC 0x%08x/0x%08x)\n",
			    (unsigned) last, (unsigned) cntr, (unsigned) crc,
			    (unsigned) expect);
		bad = 1;
		crc_errors++;
	}
	if (cntr != (inv ^ 0xffffffff)) {
		if (verbose)
			fprintf(stderr,
			    "\ninverted counter error (0x%08x->0x%08x, "
			    "inv 0x%08x)\n",
			    (unsigned) last, (unsigned) cntr, (unsigned) inv);
		bad = 1;
		inv_errors++;
	}
	if (bad)
		return 0;
	if (last > cntr)
		high++;
	last = cntr;
	s->t0 = t0.tv_sec+t0.tv_usec/1000000.0;
	s->t1 = t1.tv_sec+t1.tv_usec/1000000.0;
	s->cntr = (uint64_t) high << 32 | cntr;
	if (debug)
		printf("0x%llx 0x%lx\n", 
		    (unsigned long long ) s->cntr, (unsigned long) cntr);
	return 1;
}


static void measure(usb_dev_handle *dev, double clock_dev_s, double error_goal)
{
	struct sample start, now;
	uint64_t dc;
	double dt, f, error;
	char *f_exp, *error_exp;
	int i;

	signal(SIGINT, set_stop);

	/*
	 * The round-trip time for getting the first sample is one of the
	 * error terms. The smaller we can make it, the better. Thus, we try a
	 * few times to improve our first result.
	 */
	while (!get_sample(dev, &start));
	for (i = 0; i != 10; i++) {
		while (!get_sample(dev, &now));
		if (now.t1-now.t0 < start.t1-start.t0) {
			if (debug)
				fprintf(stderr, "improve %g -> %g\n",
				    start.t1-start.t0, now.t1-now.t0);
			start = now;
		}
	}
	while (!stop) {
		usleep(100000);
		while (!get_sample(dev, &now));
		dc = now.cntr-start.cntr;
		dt = now.t0-start.t0;
		f = dc/dt;
		if (f > 1000000.0) {
			f /= 1000000.0;
			f_exp = "M";
		} else if (f > 1000.0) {
			f /= 1000.0;
			f_exp = "k";
		} else {
			f_exp = "";
		}
		if (dc)
			error = 1.0/dc;			/* one count */
		else
			error = 0;
		error += (start.t1-start.t0)/dt;/* start sample read */
		error += (now.t1-now.t0)/dt;	/* last sample read */
		error += clock_dev_s/dt;	/* system clock deviation */
		if (error >= 1) {
			printf("\r(wait) ");
			fflush(stdout);
			continue;
		}
		if (dc && error <= error_goal)
			stop = 1;

		error_exp = "%";
		error *= 100.0;
		if (error < 0.1) {
			error_exp = " ppm";
			error *= 10000.0;
			if (error < 1.0) {
				error_exp = " ppb";
				error *= 1000.0;
			}
		}
		
		printf("\r%6.1f %1.9f %sHz %3.3f%s ",
		    dt, f, f_exp, error, error_exp);
		fflush(stdout);
	}
	printf(
	    "\n%llu counts, %u packets, %u CRC error%s, %u invert error%s\n",
	    (unsigned long long) (now.cntr-start.cntr),
	    packets, crc_errors, crc_errors == 1 ? "" : "s",
	    inv_errors, inv_errors == 1 ? "" : "s");
}


/* ----- command-line parsing ---------------------------------------------- */


static void usage(const char *name)
{
	fprintf(stderr, 
"usage: %s [-c clock_dev] [-d] [-v] [accuracy_ppm]\n"
"%6s %s -i\n"
"%6s %s -r\n\n"
"    accuracy_ppm stop when specified accuracy is reached (default: never\n"
"                 stop)\n"
"    -c clock_dev maximum deviation of the system clock, in seconds\n"
"		  (default: %g s)\n"
"    -d           debug mode. Print counter values.\n"
"    -i           identify the CNTR board\n"
"    -r           reset the CNTR board\n"
"    -v           verbose reporting of communication errors\n"
    , name, "", name, "", name, DEFAULT_CLOCK_DEV_S);
	exit(1);
}


int main(int argc, char *const *argv)
{
	usb_dev_handle *dev;
	int c, identify = 0, reset = 0;
	double clock_dev_s = DEFAULT_CLOCK_DEV_S;
	double error_goal = 0;
	char *end;

	while ((c = getopt(argc, argv, "c:dir")) != EOF)
		switch (c) {
		case 'c':
			clock_dev_s = strtod(argv[optind], &end);
			if (*end)
				usage(*argv);
			break;
		case 'd':
			debug = 1;
			break;
		case 'i':
			identify = 1;
			break;
		case 'r':
			reset = 1;
			break;
		default:
			usage(*argv);
		}
	if (identify && reset)
		usage(*argv);

	switch (argc-optind) {
	case 0:
		break;
	case 1:
		error_goal = strtod(argv[optind], &end)/1000000.0;
		if (*end)
			usage(*argv);
		break;
	default:
		usage(*argv);
	}

	dev = open_usb(USB_VENDOR, USB_PRODUCT);
	if (!dev) {
		fprintf(stderr, ":-(\n");
		return 1;
	}

	if (identify) {
		identify_cntr(dev);
		return 0;
	}

	if (reset) {
		reset_cntr(dev);
		return 0;
	}

	measure(dev, clock_dev_s, error_goal);

	return 0;
}
