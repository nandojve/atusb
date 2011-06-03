/*
 * atrf-proxy/atrf-proxy.c - ATRF network proxy
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
#include <strings.h> /* for strcasecmp */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "atrf.h"
#include "netio.h"
#include "daemon.h"


#define	DEFAULT_PORT	0x1540	/* 5440 */


static int verbose = 0;


static int get_num(struct netio *netio, int last, int *ret)
{
	char buf[10];
	ssize_t got;
	unsigned long n;
	char *end;

	got = netio_read_until(netio,
	    last ? "\n" : " ", buf, sizeof(buf), NULL);
	if (got < 0) {
		*ret = -1;
		return -1;
	}
	buf[got] = 0;
	n = strtoul(buf, &end, 0);
	if (*end) {
		*ret = netio_printf(netio, "-bad number\n");
		return -1;
	}
	/* @@@ check against INT_MAX */
	return n;
}


static int cmd_two(struct atrf_dsc *dsc, struct netio *netio, const char *cmd)
{
	int n, ret;

	n = get_num(netio, 0, &ret);
	if (n < 0)
		return ret;
	if (n > 255)
		return netio_printf(netio, "-bad argument\n");

	if (!strcasecmp(cmd, "set")) {
		int val;

		val = get_num(netio, 1, &ret);
		if (val < 0)
			return ret;
		if (val > 255)
			return netio_printf(netio, "-bad argument\n");
		atrf_reg_write(dsc, n, val);
		if (atrf_error(dsc))
			return netio_printf(netio, "-I/O error\n");
		return netio_printf(netio, "+\n");
	}
	if (!strcasecmp(cmd, "write")) {
		uint8_t *tmp;
		ssize_t got;

		if (n > 128)
			return netio_printf(netio, "-bad argument\n");
		tmp = malloc(n+1);
		if (!tmp)
			return netio_printf(netio, "-out of memory\n");
		got = netio_read(netio, tmp, n+1);
		if (got != n+1) {
			free(tmp);
			return -1;
		}
		if (tmp[n] != '\n') {
			free(tmp);
			return netio_printf(netio, "-unterminated command\n");
		}
		atrf_buf_write(dsc, tmp, n);
		free(tmp);
		if (atrf_error(dsc))
			return netio_printf(netio, "-I/O error\n");
		return netio_printf(netio, "+\n");
	}
	if (!strcasecmp(cmd, "setram")) {
		int val;

		val = get_num(netio, 1, &ret);
		if (val < 0)
			return ret;
		if (val > 255)
			return netio_printf(netio, "-bad argument\n");
		atrf_sram_write(dsc, n, val);
		if (atrf_error(dsc))
			return netio_printf(netio, "-I/O error\n");
		return netio_printf(netio, "+\n");
	}
	abort();
}


static int cmd_more(struct atrf_dsc *dsc, struct netio *netio, const char *cmd)
{
	int n, ret;

	if (!strcasecmp(cmd, "set"))
		return cmd_two(dsc, netio, cmd);
	if (!strcasecmp(cmd, "setram"))
		return cmd_two(dsc, netio, cmd);
	if (!strcasecmp(cmd, "write"))
		return cmd_two(dsc, netio, cmd);

	n = get_num(netio, 1, &ret);
	if (n < 0)
		return ret;

	if (!strcasecmp(cmd, "slp_tr")) {
		if (n > 1)
			return netio_printf(netio, "-bad argument\n");
		if (atrf_slp_tr(dsc, n) < 0)
			return netio_printf(netio, "-I/O error\n");
		return netio_printf(netio, "+\n");
	}
	if (!strcasecmp(cmd, "clkm")) {
		if (n > 16)
			return netio_printf(netio, "-bad argument\n");
		if (atrf_set_clkm(dsc, n) < 0)
			return netio_printf(netio, "-error\n");
		return netio_printf(netio, "+\n");
	}
	if (!strcasecmp(cmd, "get")) {
		uint8_t res;

		if (n > 255)
			return netio_printf(netio, "-bad argument\n");
		res = atrf_reg_read(dsc, n);
		if (atrf_error(dsc))
			return netio_printf(netio, "-I/O error\n");
		return netio_printf(netio, "+0x%02x\n", res);
	}
	if (!strcasecmp(cmd, "getram")) {
		uint8_t res;

		if (n > 255)
			return netio_printf(netio, "-bad argument\n");
		res = atrf_sram_read(dsc, n);
		if (atrf_error(dsc))
			return netio_printf(netio, "-I/O error\n");
		return netio_printf(netio, "+0x%02x\n", res);
	}
	return netio_printf(netio, "-unrecognized command\n");
}


static int cmd_zero(struct atrf_dsc *dsc, struct netio *netio, const char *cmd)
{
	int res;

	if (!strcasecmp(cmd, "spec")) {
		const char *spec = atrf_driver_spec(dsc, 1);

		if (spec)
			return netio_printf(netio, "+%s\n", spec);
		else
			return netio_printf(netio,
			    "-can't obtain specification\n");
	}
	if (!strcasecmp(cmd, "reset")) {
		atrf_reset(dsc);
		return netio_printf(netio, "+\n");
	}
	if (!strcasecmp(cmd, "reset_rf")) {
		atrf_reset_rf(dsc);
		return netio_printf(netio, "+\n");
	}
	if (!strcasecmp(cmd, "test")) {
		atrf_test_mode(dsc);
		return netio_printf(netio, "+\n");
	}
	if (!strcasecmp(cmd, "read")) {
		uint8_t buf[128+1]; /* one more for the trailing \n */
		int got;

		got = atrf_buf_read(dsc, buf, sizeof(buf));
		if (got < 0)
			return netio_printf(netio, "-I/O error\n");
		if (netio_printf(netio, "+%d ", got) < 0)
			return -1;
		buf[got] = '\n';
		return netio_write(netio, buf, got+1);
	}
	if (!strcasecmp(cmd, "poll")) {
		res = atrf_interrupt(dsc);
		if (res < 0)
			return netio_printf(netio, "-I/O error\n");
		if (!res)
			usleep(100*1000);
		return netio_printf(netio, "+%d\n", res);
	}
	return netio_printf(netio, "-unrecognized command\n");
}


static void session(const char *driver, struct netio *netio)
{
	struct atrf_dsc *dsc;

	dsc = atrf_open(driver);
	if (!dsc) {
		netio_printf(netio, "-unable to open driver\n");
		return;
	}

	if (netio_printf(netio,
	    "+connected to %s\n", driver ? driver : "default") < 0)
		goto done;

	while (1) {
		char buf[100];
		ssize_t got;
		char last;

		got = netio_read_until(netio, " \n", buf, sizeof(buf), &last);
		if (got < 0)
			break;
		if (!got && last) {
			netio_printf(netio, "-empty input\n");
			continue;
		}
		buf[got] = 0;
		switch (last) {
		case ' ':
			if (cmd_more(dsc, netio, buf) < 0)
				goto done;
			break;
		case '\n':
			if (cmd_zero(dsc, netio, buf) < 0)
				goto done;
			break;
		case 0:
			goto done;
		default:
			abort();
		}
	}
	
done:
	atrf_close(dsc);
}


static void loop(const char *driver, int port)
{
	struct sockaddr_in addr;
	int s;
	int one = 1;

	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s < 0) {
		perror("socket");
		exit(1);
	}

	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one)) < 0) {
		perror("setsockopt");
		exit(1);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("bind");
		exit(1);
	}

	if (listen(s, 0) < 0) {
		perror("listen");
		exit(1);
	}

	while (1) {
		socklen_t len = sizeof(addr);
		int s2;
		struct netio *netio;

		s2 = accept(s, (struct sockaddr *) &addr, &len);
		if (s2 < 0) {
			perror("accept");
			continue;
		}

		if (verbose)
			fprintf(stderr, "%s:%u\n", inet_ntoa(addr.sin_addr),
			  ntohs(addr.sin_port));

		netio = netio_open(s2);
		if (netio) {
			session(driver, netio);
			netio_close(netio);
		} else {
			if (close(s2) < 0)
				perror("close");
		}
	}
}


static void usage(const char *name)
{
	fprintf(stderr,
"usage: %s [-b] [-d driver[:arg]] [-v ...] [port]\n\n"
"  port             listen on the specified port (default: %d)\n\n"
"  -b               background the process after initialization\n"
"  -d driver[:arg]  use the specified driver (default: %s)\n"
"  -v ...           increase verbosity level\n"
    , name, DEFAULT_PORT, atrf_default_driver_name());
	exit(1);
}


int main(int argc, char **argv)
{
	unsigned long port = DEFAULT_PORT;
	const char *driver = NULL;
	int foreground = 1;
	char *end;
	int c;

	while ((c = getopt(argc, argv, "bd:v")) != EOF)
		switch (c) {
		case 'b':
			foreground = 0;
			break;
		case 'd':
			driver = optarg;
			break;
		case 'v':
			verbose++;
			netio_verbose++;
			break;
		default:
			usage(*argv);
		}
	
	switch (argc-optind) {
	case 0:
		break;
	case 1:
		port = strtoul(argv[optind], &end, 0);
		if (*end || !port || port > 0xffff)
			usage(*argv);
		break;
	default:
		usage(*argv);
	}

	if (foreground || !daemonize())
		loop(driver, port);

	return 0;
}
