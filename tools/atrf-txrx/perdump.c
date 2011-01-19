/*
 * atrf-txrx/perdump.c - Analyze and dump a recorded PER test
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

#include "pcap.h"
#include "perdump.h"


#define	MAX_PSDU	127


static const struct result_ops *ops;


static void analyze(uint8_t *buf, int len, double ts)
{
	static int last = -1;
	static double t0 = 0;
	int freq[256];
	uint8_t best = 0;
	int i;

	if (!t0)
		t0 = ts;
	for (i = 0; i != 256; i++)
		freq[i] = 0;
	for (i = 0; i != len; i++) {
		freq[buf[i]]++;
		if (!i || freq[buf[i]] > freq[best])
			best = buf[i];
	}
	if (freq[best] <= len >> 1 && freq[best] != len) {
		ops->undecided(len*2, ts-t0);
		if (last != -1)
			last++;	/* probably :-) */
		return;
	}
	ops->packet(len*2, last == -1 ? 0 : (uint8_t) (best-last-1), ts-t0);
	last = best;
	for (i = 0; i != len; i++) {
		uint8_t delta = buf[i] ^ best;

		if (delta & 0x0f)
			ops->error(i*2);
		if (delta & 0xf0)
			ops->error(i*2+1);
	}
	
}


static int pcap_record(FILE *file, const char *name)
{
	struct pcap_pkthdr hdr;
	uint8_t buf[MAX_PSDU];
	size_t got;

	got = fread(&hdr, sizeof(hdr), 1, file);
	if (!got) {
		if (ferror(file)) {
			perror(name);
			exit(1);
		}
		return 0;
	}
	if (hdr.caplen > MAX_PSDU) {
		fprintf(stderr, "packet too big %u > %u\n",
		    hdr.caplen, MAX_PSDU);
		exit(1);
	}
	got = fread(buf, hdr.caplen, 1, file);
	if (!got) {
		if (ferror(file)) {
			perror(name);
			exit(1);
		}
		fprintf(stderr, "file truncated\n");
		exit(1);
	}
	analyze(buf, hdr.caplen, hdr.ts_sec+hdr.ts_usec/1000000.0);
	return 1;
}


static void process_pcap(const char *name)
{
	FILE *file;
	struct pcap_file_header hdr;
	size_t got;

	file = fopen(name, "r");
	if (!file) {
		perror(name);
		exit(1);
	}
	got = fread(&hdr, sizeof(hdr), 1, file);
	if (!got) {
		if (ferror(file)) {
			perror(name);
			exit(1);
		}
		return;
		return;
	}
	if (hdr.magic != PCAP_FILE_MAGIC) {
		fprintf(stderr, "unrecognized magic number 0x%08x "
		    "(expected 0x%08x)\n", hdr.magic, PCAP_FILE_MAGIC);
		exit(1);
	}
	if (hdr.version_major != 2) {
		fprintf(stderr, "unrecognized major number %u (expected %u)\n",
		    hdr.version_major, 2);
		exit(1);
	}
	if (hdr.linktype != DLT_IEEE802_15_4) {
		fprintf(stderr, "unrecognized link type 0x%x "
		    "(expected 0x%0x)\n", hdr.linktype, DLT_IEEE802_15_4);
		exit(1);
	}
	if (ops->begin)
		ops->begin();
	while (pcap_record(file, name));
	if (ops->finish)
		ops->finish();
	fclose(file);
}


static void usage(const char *name)
{
	fprintf(stderr, "usage: %s pcap-file\n", name);
	exit(1);
}


int main(int argc, char **argv)
{
	if (argc != 2)
		usage(*argv);
	ops = &text_ops;
	process_pcap(argv[1]);
	return 0;
}
