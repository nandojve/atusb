/*
 * lib/atnet.c - Access functions library for network proxy
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "netio.h"

#include "driver.h"


#define	DEFAULT_PORT	"5440"


static char reply[1000];


static int dialog_vsend(struct netio *netio, const char *fmt, va_list ap)
{
	va_list ap2;
	char *buf;
	int n;

	va_copy(ap2, ap);
	n = vsnprintf(NULL, 0, fmt, ap2);

	buf = malloc(n+1);
	if (!buf) {
		perror("malloc");
		return -1;
	}

	vsprintf(buf, fmt, ap);

	buf[n] = '\n';
	if (netio_write(netio, buf, n+1) < 0)
		return -1;
	return 0;
}


static int dialog_send(struct netio *netio, const char *fmt, ...)
{
	va_list ap;
	int res;

	va_start(ap, fmt);
	res = dialog_vsend(netio, fmt, ap);
	va_end(ap);
	return res;
}


static int dialog_recv(struct netio *netio)
{
	int n;

	n = netio_read_until(netio, "\n", reply, sizeof(reply)-1, NULL);
	if (n < 0)
		return -1;
	reply[n] = 0;

	return reply[0] == '+' ? 0 : -1;
}


static int dialog(struct netio *netio, const char *fmt, ...)
{
	va_list ap;
	int res;

	va_start(ap, fmt);
	res = dialog_vsend(netio, fmt, ap);
	va_end(ap);
	if (res < 0)
		return res;

	return dialog_recv(netio);
}


/* ----- error handling ---------------------------------------------------- */


static int error;


static int atnet_error(void *dsc)
{
	return error;
}


static int atnet_clear_error(void *dsc)
{
	int ret;

	ret = error;
	error = 0;
	return ret;
}


/* ----- open/close -------------------------------------------------------- */


static void *atnet_open(const char *arg)
{
	char *host = NULL, *comma;
	const char *port = DEFAULT_PORT;
	struct addrinfo *addrs;
	const struct addrinfo *addr;
	static struct addrinfo hint = { .ai_socktype = SOCK_STREAM };
	int res, s;
	struct netio *netio;

	if (arg) {
		host = strdup(arg);
		if (!host) {
			perror("strdup");
			return NULL;
		}
		comma = strchr(host, ',');
		if (comma) {
			*comma = 0;
			port = comma+1;
		}
	}
	res = getaddrinfo(arg && *host ? host : NULL, port, &hint, &addrs);
	if (res < 0) {
		fprintf(stderr, "%s: %s\n", arg, gai_strerror(res));
		free(host);
		return NULL;
	}
	free(host);

	for (addr = addrs; addr; addr = addr->ai_next) {
		s = socket(addr->ai_family, addr->ai_socktype,
		    addr->ai_protocol);
		if (s < 0)
			continue;
		if (connect(s, addr->ai_addr, addr->ai_addrlen) >= 0)
			break;
		close(s);
	}
	if (!addr) {
		perror(arg);
		freeaddrinfo(addrs);
		return NULL;
	}

	freeaddrinfo(addrs);

	netio = netio_open(s);
	if (!netio)
		return NULL;

	if (dialog_recv(netio) < 0) {
		netio_close(netio);
		return NULL;
	}

	error = 0;
	return netio;
}


static void atnet_close(void *dsc)
{
	struct netio *netio = dsc;

	netio_close(netio);
}


/* ----- device mode ------------------------------------------------------- */


static void atnet_reset(void *dsc)
{
	struct netio *netio = dsc;

	if (error)
		return;
	if (dialog(netio, "RESET") < 0)
		error = 1;
}


static void atnet_reset_rf(void *dsc)
{
	struct netio *netio = dsc;

	if (error)
		return;
	if (dialog(netio, "RESET_RF") < 0)
		error = 1;
}


static void atnet_test_mode(void *dsc)
{
	struct netio *netio = dsc;

	if (error)
		return;
	if (dialog(netio, "TEST") < 0)
		error = 1;
}


static void atnet_slp_tr(void *dsc, int on)
{
	struct netio *netio = dsc;

	if (error)
		return;
	if (dialog(netio, "SLP_TR %d", on) < 0)
		error = 1;
}


/* ----- register access --------------------------------------------------- */


static void atnet_reg_write(void *dsc, uint8_t reg, uint8_t value)
{
	struct netio *netio = dsc;

	if (error)
		return;
	if (dialog(netio, "SET 0x%02x 0x%02x", reg, value) < 0)
		error = 1;
}


static uint8_t atnet_reg_read(void *dsc, uint8_t reg)
{
	struct netio *netio = dsc;
	unsigned long value;
	char *end;

	if (error)
		return;
	if (dialog(netio, "GET 0x%02x", reg) < 0) {
		error = 1;
		return 0;
	}
	value = strtoul(reply+1, &end, 0);
	if (*end || value > 255) {
		fprintf(stderr, "invalid response \"%s\"\n", reply);
		error = 1;
		return 0;
	}
	return value;
}


/* ----- frame buffer access ----------------------------------------------- */


static void atnet_buf_write(void *dsc, const void *buf, int size)
{
	struct netio *netio = dsc;
	char tmp[20];
	int n;

	if (error)
		return;

	n = snprintf(tmp, sizeof(tmp), "WRITE %d ", size);
	assert(n < sizeof(tmp));
	if (netio_write(netio, tmp, n) < 0) {
		error = 1;
                return;
	}

	if (netio_write(netio, buf, size) < 0) {
		error = 1;
                return;
	}

	if (netio_write(netio, "\n", 1) < 0) {
		error = 1;
                return;
	}

	if (dialog_recv(netio) < 0)
		error = 1;
}


static int atnet_buf_read(void *dsc, void *buf, int size)
{
	struct netio *netio = dsc;
	uint8_t tmp[200];
	int n, got = 0;
	unsigned long len = 0;
	char *end;

	if (error)
		return -1;

	if (dialog_send(netio, "READ") < 0)
		goto fail;

	n = netio_read_until(netio, " ", tmp, sizeof(tmp)-1, NULL);
	if (*tmp == '-') {
		tmp[n] = 0;
		fprintf(stderr, "%s\n", tmp+1);
		goto fail;
	}
	if (*tmp != '+' || n < 3) /* +0<spc> */
		goto invalid;
	len = strtoul(tmp+1, &end, 0);
	if (*end != ' ')
		goto invalid;
	if (len > size) {
		fprintf(stderr, "buffer overflow\n");
		goto fail;
	}

	got = netio_read(netio, buf, len);
	if (got < 0)
		goto fail;

	if (netio_getc(netio, tmp) < 0)
		goto fail;
	if (*tmp == '\n')
		return len;

invalid:
	fprintf(stderr, "invalid reponse\n");
fail:
	error = 1;
	return -1;
}


/* ----- RF interrupt ------------------------------------------------------ */


static int atnet_interrupt(void *dsc)
{
	struct netio *netio = dsc;
	unsigned long value;
	char *end;

	if (error)
		return -1;
	if (dialog(netio, "POLL") < 0) {
		error = 1;
		return -1;
	}
	value = strtoul(reply+1, &end, 0);
	if (*end || value > 1) {
		fprintf(stderr, "invalid response \"%s\"\n", reply);
		error = 1;
		return -1;
	}
	return value;
}


/* ----- CLKM handling ----------------------------------------------------- */


static int atnet_set_clkm(void *dsc, int mhz)
{
	struct netio *netio = dsc;

	return dialog(netio, "CLKM %d", mhz);
}


/* ----- driver interface -------------------------------------------------- */


struct atrf_driver atnet_driver = {
	.name		= "net",
	.open		= atnet_open,
	.close		= atnet_close,
	.error		= atnet_error,
	.clear_error	= atnet_clear_error,
	.reset		= atnet_reset,
	.reset_rf	= atnet_reset_rf,
	.test_mode	= atnet_test_mode,
	.slp_tr		= atnet_slp_tr,
	.set_clkm	= atnet_set_clkm,
	.reg_write	= atnet_reg_write,
	.reg_read	= atnet_reg_read,
	.buf_write	= atnet_buf_write,
	.buf_read	= atnet_buf_read,
	.interrupt	= atnet_interrupt,
};
