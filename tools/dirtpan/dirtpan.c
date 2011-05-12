/*
 * dirtpan/dirtpan.c - Quick and dirty IPv4 over 802.15.4 tunnel
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
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if.h>
#include <linux/if_tun.h>

#include <ieee802154.h>


/*
 * Control byte structure:
 *
 * +--7--+--6--+--5--+--4--+--3--+--2--+--1--+
 * |  0  |  0  |  0  |  0  | seq | pck_type  |
 * +-----+-----+-----+-----+-----+-----+-----+
 */

#define	SEQ	4
#define	PT_MASK	3

enum packet_type {
	pt_first	= 0,
	pt_next		= 1,
	pt_ack		= 2,
};


#define	TUN_DEV	"/dev/net/tun"

#define	MAX_FRAG	(127-11-2-1)	/* MHDR, FCS, control byte */
#define	MAX_PACKET	2000
#define	MAX_TRIES	5
#define	T_REASS_MS	200
#define	T_ACK_MS	50


static int tun, net;
static uint8_t rx_packet[MAX_PACKET], tx_packet[MAX_PACKET+1];
static void *rx_pos, *tx_pos;
static int rx_left, tx_left;
static int txing = 0, rxing = 0;
static int rx_seq, tx_seq = 0;
static int retries;
static int debug = 0;


/* ----- Debugging --------------------------------------------------------- */


static void debug_label(const char *label)
{
	fprintf(stderr, "%s(%c%c)",
	    label, txing ? 'T' : '-', rxing ? 'R' : '-');
}


static void dump(const void *buf, int size)
{
	const uint8_t *p = buf;

	while (size--) {
		fprintf(stderr, "%s%02x", p == buf ? "" : " ", *p);
		p++;
	}
}


static void debug_ip(const char *label, void *buf, int size)
{
	if (debug < 2)
		return;
	debug_label(label);
	fprintf(stderr, ", %d: ", size);
	dump(buf, size);
	fprintf(stderr, "\n");
}


static void debug_dirt(const char *label, void *buf, int size)
{
	const uint8_t *p = buf;

	if (!debug)
		return;
	if (debug == 1) {
		if (size) {
			fprintf(stderr, "%c%d",
			    (label[1] == '>' ? "FNA?" : "fna?")[*p & PT_MASK],
			    *p & SEQ ? 0 : 1);
		}
		return;
	}
	debug_label(label);
	fprintf(stderr, ", %d+1: ", size-1);
	if (size) {
		fprintf(stderr, "%02x(%c%d) | ",
		    *p, "FNA?"[*p & PT_MASK], *p & SEQ ? 0 : 1);
		dump(buf+1, size-1);
	}
	fprintf(stderr, "\n");

}


static void debug_timeout(const char *label)
{
	if (!debug)
		return;
	if (debug == 1) {
		fprintf(stderr, "*%c", *label);
		return;
	}
	debug_label(label);
	fprintf(stderr, "\n");
}


/* ----- Timers ------------------------------------------------------------ */


static struct timeval t_reass, t_ack;


static void start_timer(struct timeval *t, int ms)
{
	assert(!t->tv_sec && !t->tv_usec);
	gettimeofday(t, NULL);
	t->tv_usec += 1000*ms;
	while (t->tv_usec >= 1000000) {
		t->tv_sec++;
		t->tv_usec -= 1000000;
	}
}


static void stop_timer(struct timeval *t)
{
	assert(t->tv_sec || t->tv_usec);
	t->tv_sec = 0;
	t->tv_usec = 0;
}


static const struct timeval *next_timer(int n, ...)
{
	va_list ap;
	const struct timeval *next = NULL;
	const struct timeval *t;

	va_start(ap, n);
	while (n--) {
		t = va_arg(ap, const struct timeval *);
		if (!t->tv_sec && !t->tv_usec)
			continue;
		if (next) {
			if (next->tv_sec < t->tv_sec)
				continue;
			if (next->tv_sec == t->tv_sec &&
			    next->tv_usec < t->tv_usec)
				continue;
		}
		next = t;
	}
	va_end(ap);
	return next;
}


static struct timeval *timer_delta(const struct timeval *t)
{
	static struct timeval d;

	if (!t)
		return NULL;

	gettimeofday(&d, NULL);
	d.tv_sec = t->tv_sec-d.tv_sec;
	d.tv_usec = t->tv_usec-d.tv_usec;

	while (d.tv_usec < 0) {
		d.tv_sec--;
		d.tv_usec += 1000000;
	}
	if (d.tv_sec < 0)
		d.tv_sec = d.tv_usec = 0;

	return &d;
}


/* ----- Packet/frame delivery --------------------------------------------- */


static inline int send_size(void)
{
	return tx_left > MAX_FRAG ? MAX_FRAG : tx_left;
}


static void write_buf(int fd, void *buf, int size)
{
	ssize_t wrote;

	wrote = write(fd, buf, size);
	if (wrote < 0) {
		perror("write");
		return;
	}
	if (wrote != size)
		fprintf(stderr, "short write: %d < %d\n", (int) wrote, size);
}


static void send_frame(void *buf, int size)
{
	debug_dirt("->", buf, size);
	write_buf(net, buf, size);
}


static void send_more(void)
{
	uint8_t *p = tx_pos-1;

	*p = (tx_pos == tx_packet+1 ? pt_first : pt_next) | (tx_seq ? SEQ : 0);
	send_frame(p, send_size()+1);
	start_timer(&t_ack, T_ACK_MS);
}


static void send_ack(int seq)
{
	uint8_t ack = pt_ack | (seq ? SEQ : 0);

	send_frame(&ack, 1);
}


/* ----- Main events ------------------------------------------------------- */


static void rx_pck(void *buf, int size)
{
	const uint8_t *p = buf;
	uint8_t ctrl, type, seq;

	debug_dirt("-<", buf, size);

	if (size < 1)
		return;

	ctrl = *p;
	type = ctrl & PT_MASK;
	seq = !!(ctrl & SEQ);

	switch (type) {
	case pt_first:
		send_ack(seq);
		if (rxing) {
			stop_timer(&t_reass);
			rxing = 0;
		}
		break;
	case pt_next:
		send_ack(seq);
		if (!rxing)
			return;
		if (seq == rx_seq)
			return; /* retransmission */
		break;
	case pt_ack:
		if (!txing)
			return;
		if (seq != tx_seq)
			return;
		stop_timer(&t_ack);
		tx_pos += send_size();
		tx_left -= send_size();
		if (!tx_left) {
			txing = 0;
			return;
		}
		tx_seq = !tx_seq;
		retries = 0;
		send_more();
		return;
	default:
		abort();
	}

	if (!rxing) {
		if (size < 5)
			return;
		rx_left = p[3] << 8 | p[4];
		if (rx_left > MAX_PACKET)
			return;
		start_timer(&t_reass, T_REASS_MS);
		rxing = 1;
		rx_pos = rx_packet;
	}

	if (rx_left < size-1) {
		stop_timer(&t_reass);
		rxing = 0;
		return;
	}
	memcpy(rx_pos, buf+1, size-1);
	rx_pos += size-1;
	rx_left -= size-1;
	rx_seq = seq;

	if (!rx_left) {
		debug_ip("<-", rx_packet, rx_pos-(void *) rx_packet);
		write_buf(tun, rx_packet, rx_pos-(void *) rx_packet);
		stop_timer(&t_reass);
		rxing = 0;
	}
}


static void tx_pck(void *buf, int size)
{
	const uint8_t *p = buf;

	debug_ip(">-", buf, size);
	assert(!txing);
	txing = 1;
	tx_pos = tx_packet+1;
	tx_left = p[2] << 8 | p[3];
	assert(tx_left <= MAX_PACKET);
	assert(tx_left == size);
	/*
	 * @@@ We could avoid the memcpy by reading directly into "tx_packet"
	 */
	memcpy(tx_pos, buf, size);
	tx_seq = !tx_seq;
	retries = 0;
	send_more();
}


static void ack_timeout(void)
{
	debug_timeout("ACK-TO");
	assert(txing);
	stop_timer(&t_ack);
	if (++retries == MAX_TRIES)
		txing = 0;
	else
		send_more();
}


static void reass_timeout(void)
{
	debug_timeout("REASS-TO");
	assert(rxing);
	stop_timer(&t_reass);
	rxing = 0;
}


/* ----- Event dispatcher -------------------------------------------------- */


static void event(void)
{
	uint8_t buf[MAX_PACKET];
	const struct timeval *to;
	fd_set rset;
	int res;
	ssize_t got;

	FD_ZERO(&rset);
	FD_SET(net, &rset);

	/* only accept more work if we're idle */
	if (!txing && !rxing)
		FD_SET(tun, &rset);

	to = next_timer(2, &t_reass, &t_ack);

	res = select(net > tun ? net+1 : tun+1, &rset, NULL, NULL,
	    timer_delta(to));
	if (res < 0) {
		perror("select");
		return;
	}
	if (!res) {
		assert(to);
		if (to == &t_reass)
			reass_timeout();
		else
			ack_timeout();
	}
	if (FD_ISSET(tun, &rset)) {
		got = read(tun, buf, sizeof(buf));
		if (got < 0) {
			perror("read tun");
			return;
		}
		tx_pck(buf, got);
	}
	if (FD_ISSET(net, &rset)) {
		got = read(net, buf, sizeof(buf));
		if (got < 0) {
			perror("read net");
			return;
		}
		rx_pck(buf, got);
	}
}


/* ----- Setup ------------------------------------------------------------- */


static int open_net(uint16_t pan, uint16_t me, uint16_t peer)
{
	struct sockaddr_ieee802154 addr;
	int zero = 0;
	int s;

	s = socket(PF_IEEE802154, SOCK_DGRAM, 0);
	if (s < 0) {
		perror("socket 802.15.4");
		exit(1);
	}

	addr.family = AF_IEEE802154;
	addr.addr.addr_type = IEEE802154_ADDR_SHORT;
	addr.addr.pan_id = pan;
	addr.addr.short_addr = me;

	if (bind(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("bind 802.15.4");
		exit(1);
	}

	addr.addr.short_addr = peer;
	if (connect(s, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		perror("connect 802.15.4");
		exit(1);
	}

	if (setsockopt(s, SOL_IEEE802154, WPAN_WANTACK, &zero, sizeof(zero))
	    < 0) {
		perror("setsockopt SOL_IEEE802154 WPAN_WANTACK");
		exit(1);
	}

	return s;
}


static int open_tun(const char *cmd)
{
	struct ifreq ifr;
	int fd, res;

	fd = open(TUN_DEV, O_RDWR);
	if (fd < 0) {
		perror(TUN_DEV);
		exit(1);
	}

	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = IFF_TUN | IFF_NO_PI;

	if (ioctl(fd, TUNSETIFF, (void *) &ifr) < 0) {
		perror("ioctl TUNSETIFF");
		exit(1);
	}

	if (!cmd) {
		fprintf(stderr, "%s\n", ifr.ifr_name);
		return fd;
	}

	if (setenv("ITF", ifr.ifr_name, 1) < 0) {
		perror("setenv");
		exit(1);
	}

	res = system(cmd);
	if (res < 0) {
		perror("system");
		exit(1);
	}
	if (WIFEXITED(res)) {
		if (!WEXITSTATUS(res))
			return fd;
		exit(WEXITSTATUS(res));
	}
	if (WIFSIGNALED(res)) {
		raise(WTERMSIG(res));
		exit(1);
	}

	fprintf(stderr, "cryptic exit status %d\n", res);
	exit(1);
}


/* ----- Command-line processing ------------------------------------------- */


static void usage(const char *name)
{
	fprintf(stderr,
"usage: %s [-d [-d]] pan_id src_addr dst_addr [command]\n"
    , name);
	exit(1);
}


static uint16_t addr(const char *name, const char *s)
{
	char *end;
	unsigned long n;

	n = strtoul(s, &end, 16);
	if (*end)
		usage(name);
	if (n > 0xffff)
		usage(name);
	return n;
}


int main(int argc, char **argv)
{
	const char *cmd = NULL;
	uint16_t pan, src, dst;
	int c;

	while ((c = getopt(argc, argv, "d")) != EOF)
		switch (c) {
		case 'd':
			debug++;
			break;
		default:
			usage(*argv);
		}

	switch (argc-optind) {
	case 4:
		cmd = argv[optind+3];
		/* fall through */
	case 3:
		pan = addr(*argv, argv[optind]);
		src = addr(*argv, argv[optind+1]);
		dst = addr(*argv, argv[optind+2]);
		break;
	default:
		usage(*argv);
	}

	net = open_net(pan, src, dst);
	tun = open_tun(cmd);
	while (1)
		event();

	return 0;
}
