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


static enum state {
	s_idle,
	s_tx,
	s_rx,
} state = s_idle;


static int tun, net;
static uint8_t packet[MAX_PACKET+1];
static void *pos;
static int left;
static int my_seq = 0, peer_seq;
static int retries;
static int debug = 0;


/* ----- Debugging --------------------------------------------------------- */


static void debug_label(const char *label)
{
	const char *t;

	switch (state) {
	case s_idle:
		t = "--";
		break;
	case s_tx:
		t = "tx";
		break;
	case s_rx:
		t = "rx";
		break;
	default:
		t = "???";
		break;
	}
	fprintf(stderr, "%s(%s)", label, t);
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


static struct timeval t0;


static void start_timer(void)
{
	gettimeofday(&t0, NULL);
}


static struct timeval *timeout(int ms)
{
	static struct timeval t;

	gettimeofday(&t, NULL);
	t.tv_sec -= t0.tv_sec;
	t.tv_usec -= t0.tv_usec;
	t.tv_usec += 1000*ms;

	while (t.tv_usec < 0) {
		t.tv_sec--;
		t.tv_usec += 1000000;
	}
	while (t.tv_usec >= 1000000) {
		t.tv_sec++;
		t.tv_usec -= 1000000;
	}
	if (t.tv_sec < 0)
		t.tv_sec = t.tv_usec = 0;

	return &t;
}


/* ----- Packet/frame delivery --------------------------------------------- */


static inline int send_size(void)
{
	return left > MAX_FRAG ? MAX_FRAG : left;
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
	uint8_t *p = pos-1;

	*p = (pos == packet+1 ? pt_first : pt_next) | (my_seq ? SEQ : 0);
	send_frame(p, send_size()+1);
	start_timer();
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

	if (type == pt_first || type == pt_next)
		send_ack(seq);
	switch (state) {
	case s_tx:
		if (type == pt_first) {
			/*
			 * @@@ Not optimal - we break the tie but lose a
			 * perfectly good frame.
			 */
			state = s_idle;
			return;
		}
		if (type != pt_ack)
			return;
		if (seq != my_seq)
			return;
		pos += send_size();
		left -= send_size();
		if (!left) {
			state = s_idle;
			return;
		}
		my_seq = !my_seq;
		retries = 0;
		send_more();
		return;
	case s_rx:
		if (type == pt_first) {
			start_timer();
			state = s_idle;
			break;
		}
		if (type != pt_next)
			return;
		if (seq == peer_seq)
			return; /* retransmission */
		goto recv_more;
	case s_idle:
		if (type == pt_first)
			break;
		if (type == pt_next)
			return;
		return;
	default:
		abort();
	}

	if (size < 5)
		return;
	left = p[3] << 8 | p[4];
	if (left > MAX_PACKET+1)
		return;
	state = s_rx;
	pos = packet;

recv_more:
	if (left < size-1) {
		state = s_idle;
		return;
	}
	memcpy(pos, buf+1, size-1);
	pos += size-1;
	left -= size-1;
	peer_seq = seq;

	if (!left) {
		debug_ip("<-", packet, pos-(void *) packet);
		write_buf(tun, packet, pos-(void *) packet);
		state = s_idle;
	}
}


static void tx_pck(void *buf, int size)
{
	const uint8_t *p = buf;

	debug_ip(">-", buf, size);
	assert(state == s_idle);
	state = s_tx;
	pos = packet+1;
	left = p[2] << 8 | p[3];
	assert(left <= MAX_PACKET);
	assert(left == size);
	/*
	 * We could avoid the memcpy by reading directly into "packet"
	 */
	memcpy(pos, buf, size);
	my_seq = !my_seq;
	retries = 0;
	send_more();
}


static void ack_timeout(void)
{
	debug_timeout("ACK-TO");
	if (++retries == MAX_TRIES)
		state = s_idle;
	else
		send_more();
}


static void reass_timeout(void)
{
	debug_timeout("REASS-TO");
	state = s_idle;
}


/* ----- Event dispatcher -------------------------------------------------- */


static void event(void)
{
	uint8_t buf[MAX_PACKET];
	struct timeval *to;
	fd_set rset;
	int res;
	ssize_t got;

	FD_ZERO(&rset);
	FD_SET(net, &rset);
	switch (state) {
	case s_idle:
		FD_SET(tun, &rset);
		to = NULL;
		break;
	case s_rx:
		to = timeout(T_REASS_MS);
		break;
	case s_tx:
		to = timeout(T_ACK_MS);
		break;
	default:
		abort();
	}
	res = select(net > tun ? net+1 : tun+1, &rset, NULL, NULL, to);
	if (res < 0) {
		perror("select");
		return;
	}
	if (!res) {
		switch (state) {
		case s_rx:
			reass_timeout();
			break;
		case s_tx:
			ack_timeout();
			break;
		default:
			abort();
		}
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
