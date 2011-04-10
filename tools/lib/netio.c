/*
 * lib/netio.c - Helper functions for socket I/O
 *
 * Written 2011 by Werner Almesberger
 * Copyright 2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#include "netio.h"


#define	BUFFER	1024


struct netio {
	int s;
	uint8_t *pos;
	int size;
	uint8_t buf[BUFFER];
};


int netio_verbose = 0;


static void display(uint8_t c)
{
	fputc((c >= ' ' && c <= '~') || c == '\n' ? c : '?', stderr);
}


int netio_getc(struct netio *dsc, uint8_t *res)
{
	ssize_t got;

	while (!dsc->size) {
		got = read(dsc->s, dsc->buf, BUFFER);
		if (got < 0) {
			perror("read");
			return -1;
		}
		if (!got)
			return 0;
		dsc->pos = dsc->buf;
		dsc->size = got;
	}
	dsc->size--;
	*res = *dsc->pos++;
	if (netio_verbose)
		display(*res);
	return 1;
}


ssize_t netio_read_until(struct netio *dsc, const char *end, void *buf,
    size_t len, char *last)
{
	size_t left;
	const char *term;

	for (left = len; left; left--)
		switch (netio_getc(dsc, buf)) {
		case 1:
			buf++;
			term = strchr(end, ((char *) buf)[-1]);
			if (!term)
				break;
			if (!*term) {
				fprintf(stderr, "received NUL in text\n");
				return -1;
			}
			if (last)
				*last = ((char *) buf)[-1];
			return len-left; /* we don't count the terminator */
		case 0:
			if (last)
				*last = 0;
			return len-left;
		case -1:
			return -1;
		}
	fprintf(stderr, "buffer overrun\n");
	return -1;
}


ssize_t netio_read(struct netio *dsc, void *buf, size_t len)
{
	size_t left;

	for (left = len; left; left--)
		switch (netio_getc(dsc, buf)) {
		case 1:
			buf++;
			break;
		case 0:
			return len-left;
		case -1:
			return -1;
		}
	return len;
}


int netio_write(struct netio *dsc, const void *data, size_t len)
{
	const uint8_t *p;
	ssize_t left, wrote;

	if (netio_verbose)
		for (p = data; p != data+len; p++)
			display(*p);
	for (left = len; left; left -= wrote) {
		wrote = write(dsc->s, data, len);
		if (wrote < 0) {
			perror("write");
			return -1;
		}
		data += wrote;
	}
	return len;
}


int netio_printf(struct netio *dsc, const char *fmt, ...)
{
	va_list ap;
	int n, res;
	char *buf;

	va_start(ap, fmt);
	n = vsnprintf(NULL, 0, fmt, ap);
	va_end(ap);

	buf = malloc(n+1);
	if (!buf) {
		perror("malloc");
		return -1;
        }

	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);

	res = netio_write(dsc, buf, n);

	free(buf);

	return res;
}


struct netio *netio_open(int s)
{
	struct netio *dsc;

	dsc = malloc(sizeof(*dsc));
	if (!dsc) {
		perror("malloc");
		return NULL;
	}
	dsc->s = s;
	dsc->size = 0;
	return dsc;
}


void netio_close(struct netio *dsc)
{
	if (close(dsc->s) < 0)
		perror("close");
	free(dsc);
}
