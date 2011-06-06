/*
 * atrf-gpio/atrf-gpio.c - ATBEN/ATUSB GPIO test
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
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <errno.h>

#include "at86rf230.h"
#include "atrf.h"

#include "atrf-gpio.h"


#define	DEFAULT_DELAY_MS	10


/* ----- Board-specific drivers -------------------------------------------- */


static void atben(struct atrf_dsc *dsc, const char *pattern, const char *next)
{
#ifdef HAVE_ATBEN
	do_atben(dsc, pattern, next);
#else
	fprintf(stderr, "not compiled with ATBEN support\n");
	exit(1);
#endif
}


static void atusb(struct atrf_dsc *dsc, const char *pattern, const char *next)
{
#ifdef HAVE_ATUSB
	do_atusb(dsc, pattern, next);
#else
	fprintf(stderr, "not compiled with ATUSB support\n");
	exit(1);
#endif
}


/* ----- Commands ---------------------------------------------------------- */


static void bad_reg_op(const char *arg)
{
	fprintf(stderr, "invalid operation \"%s\"\n", arg);
	exit(1);
}


static int reg_op(struct atrf_dsc *dsc, const char *arg, int doit)
{
	const char *p;
	char *end;
	unsigned long reg, value, mask = 0xff;
	uint8_t got;

	if (!strcmp(arg, "delay")) {
		if (doit)
			usleep(DEFAULT_DELAY_MS*1000);
		return 1;
	}
	if (!strncmp(arg, "delay=", 6)) {
		value = strtoul(arg+6, &end, 0);
		if (!value || *end)
			bad_reg_op(arg);
		if (doit)
			usleep(value*1000);
		return 1;
	}
	if (!strcmp(arg, "frame")) {
		if (doit)
			atrf_buf_write(dsc, "", 1);
		return 1;
	}
	if (!strcmp(arg, "reset")) {
		if (doit)
			atrf_reset_rf(dsc);
		return 1;
	}
	if (!strcmp(arg, "slp_tr")) {
		if (doit)
			atrf_slp_tr(dsc, 1, 1);
		return 1;
	}

	p = strchr(arg, '=');
	if (!p)
		p = strchr(arg, ':');
	if (!p)
		p = strchr(arg, '!');
	if (!p)
		p = strchr(arg, '/');
	if (!p)
		return 0;
	reg = strtoul(arg, &end, 0);
	if (end != p || reg > 0xff)
		bad_reg_op(arg);
	value = strtoul(p+1, &end, 0);
	if (value > 0xff)
		bad_reg_op(arg);
	if (*end) {
		if (*p != ':')
			bad_reg_op(arg);
		if (*end != '/')
			bad_reg_op(arg);
		mask = strtoul(end+1, &end, 0);
		if (*end || mask > 0xff)
			bad_reg_op(arg);
	}

	if (!doit)
		return 1;

	switch (*p) {
	case '=':
		atrf_reg_write(dsc, reg, value);
		break;
	case ':':
		got = atrf_reg_read(dsc, reg);
		if (end != p+1 && ((got ^ value) & mask)) {
			fprintf(stderr,
			    "register 0x%02lx: got 0x%02x expected "
			    "0x%02lx/0x%02lx\n", reg, got, value, mask);
			exit(1);
		}
		break;
	case '!':
		atrf_sram_write(dsc, reg, value);
		break;
	case '/':
		got = atrf_sram_read(dsc, reg);
		if (got != value) {
			fprintf(stderr,
			    "got 0x%02x expected 0x%02lx\n", got, value);
			exit(1);
		}
		break;
	default:
		abort();
	}
	return 1;
}


/* ----- Pass/Fail/Quit input ---------------------------------------------- */


static struct termios old_term;


static void restore_term(void)
{
	if (tcsetattr(0, TCSAFLUSH, &old_term) < 0)
		perror("tcsetattr");
}


static void raw(void)
{
	struct termios term;

	if (tcgetattr(0, &old_term) < 0) {
		perror("tcgetattr");
		exit(1);
	}
	term = old_term;
	cfmakeraw(&term);
	if (tcsetattr(0, TCSAFLUSH, &term) < 0) {
		perror("tcsetattr");
		exit(1);
	}
	atexit(restore_term);
	if (fcntl(0, F_SETFL, O_NONBLOCK) < 0) {
		perror("fcntl");
		exit(1);
	}
}


static void pass_fail(void)
{
	ssize_t got;
	char ch;

	got = read(0, &ch, 1);
	if (got < 0) {
		if (errno == EAGAIN)
			return;
		perror("read");
		exit(1);
	}
	switch (ch) {
	case 'P':
	case 'p':
		exit(0);
	case 'F':
	case 'f':
	case 'Q':
	case 'q':
	case 3:	/* Ctrl-C */
		exit(1);
	default:
		break;
	}
}


/* ----- Command line processing and main loop ----------------------------- */


static void usage(const char *name)
{
	fprintf(stderr,
"usage: %s [-c] [-d driver[:arg]] command|pattern ...\n"
"  -c               cycle, waiting for Pass/Fail/Quit input\n"
"  -d driver[:arg]  use the specified driver (default: %s)\n\n"
"  command is one of:\n"
"    reg=value    set transceiver register\n"
"    reg:[value[/mask]]\n"
"                 read transceiver register and (optionally) verify value\n"
"    addr!value   write one byte to SRAM\n"
"    addr/value   read and verify one byte from SRAM\n"
"    delay[=ms]   wait the specified number of milliseconds (default: %d ms)\n"
"    frame        write a one-byte frame to the frame buffer\n"
"    reset        reset the transceiver\n"
"    slp_tr       pulse SLP_TR\n"
"    #...         comment\n\n"
"  pattern is a sequence of the following characters:\n"
"    0 = output a strong 0               1 = output a strong 1\n"
"    L = pull up, expect to read 0       H = pull up, expect to read 1\n"
"    l/o = no pull-up, expect to read 0  h = no pull-up, expect to read 1\n"
"    Z = pull up, don't read             z = no pull-up, don't read\n"
"    x = don't care                      . = separator\n"
    , name, atrf_default_driver_name(), DEFAULT_DELAY_MS);
	exit(1);
}


/*
 * 0	strong 0 out
 * 1	strong 1 out
 * H	pull-up, read 1
 * L	pull-up, read 0
 * h	no pull-up, read 1
 * l/o	no pull-up, read 0
 * Z	pull-up, don't read
 * z	no pull-up, don't read
 * x	don't care
 * .	separator
 */


int main(int argc, char *const *argv)
{
	const char *driver = NULL;
	struct atrf_dsc *dsc;
	int cycle = 0;
	int trx_off = 1;
	int c, i;
	const char *s;

	while ((c = getopt(argc, argv, "cd:p")) != EOF)
		switch (c) {
		case 'c':
			cycle = 1;
			break;
		case 'd':
			driver = optarg;
			break;
		case 'p':
			trx_off = 0;
			break;
		default:
			usage(*argv);
		}

	for (i = optind; i != argc; i++) {
		if (*argv[i] == '#')
			continue;
		if (reg_op(NULL, argv[i], 0))
			continue;
		for (s = argv[i]; *s; s++)
			if (!strchr("01HLhloZzx.", *s))
				fprintf(stderr,
				    "invalid configuration '%c' in \"%s\"\n",
				    *s, argv[i]);
	}

	dsc = atrf_open(driver);
	if (!dsc)
		return 1;

	atrf_reset_rf(dsc);

	if (trx_off) {
		atrf_reg_write(dsc, REG_TRX_STATE, TRX_CMD_TRX_OFF);
		do usleep(100);
		while ((atrf_reg_read(dsc, REG_TRX_STATUS) & TRX_STATUS_MASK)
		    != TRX_STATUS_TRX_OFF);
	}

	if (cycle)
		raw();

	while (1) {
		for (i = optind; i != argc; i++) {
			if (*argv[i] == '#')
				continue;
			if (reg_op(dsc, argv[i], 1))
				continue;
			if (atrf_usb_handle(dsc))
				atusb(dsc, argv[i], argv[i+1]);
			else
				atben(dsc, argv[i], argv[i+1]);
		}
		if (cycle)
			pass_fail();
		else
			break;
	}

//	atrf_close(dsc);

	return 0;
}
