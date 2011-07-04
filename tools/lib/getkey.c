/*
 * lib/getkey.c - Get single characters from standard input
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
#include <fcntl.h>
#include <termios.h>
#include <errno.h>



static struct termios old_term;


static void restore_term(void)
{
	if (tcsetattr(0, TCSAFLUSH, &old_term) < 0)
		perror("tcsetattr");
}


void get_key_init(void)
{
	static int initialized = 0;
	struct termios term;

	if (initialized)
		return;
	initialized = 1;

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


char get_key(void)
{
	ssize_t got;
        char ch;

	get_key_init();
	got = read(0, &ch, 1);
	if (got == 1)
		return ch;
	if (got >= 0) {
		fprintf(stderr, "unexpected read() return value %d\n",
		    (int) got);
		exit(1);
	}
	if (errno == EAGAIN)
		return 0;
	perror("read");
	exit(1);
}
