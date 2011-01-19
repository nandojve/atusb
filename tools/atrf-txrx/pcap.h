/*
 * pcap.h - Minimum pcap file definitions
 *
 * Written 2011 by Werner Almesberger
 * Copyright 2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

/*
 * This header defines the few things we need to write files in the pcap
 * format. The identifiers are the same as in the system's pcap/pcap.h, but
 * the types have been standardized. Note that the timestamp parts have to be
 * put separately, since "struct timeval" may be padded.
 *
 * The reason for having our own header instead of just using pcap/pcap.h is
 * to avoid a build-dependency on libpcap.
 */

#ifndef PCAP_H
#define	PCAP_H

#include <stdint.h>
#include <sys/time.h>


#define	PCAP_FILE_MAGIC		0xa1b2c3d4

#define	DLT_IEEE802_15_4	195


struct pcap_file_header {
	uint32_t magic;
	uint16_t version_major;
	uint16_t version_minor;
	int32_t thiszone;
	uint32_t sigfigs;
	uint32_t snaplen;
	uint32_t linktype;
};

struct pcap_pkthdr {
	uint32_t ts_sec;
	uint32_t ts_usec;
	uint32_t caplen;
	uint32_t len;
};

#endif /* !PCAP_H */
