/*
 * include/netio.h - Helper functions for socket I/O
 *
 * Written 2011 by Werner Almesberger
 * Copyright 2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef NETIO_H
#define	NETIO_H

#include <stdint.h>
#include <sys/types.h>


struct netio;

extern int netio_verbose;

int netio_getc(struct netio *dsc, uint8_t *res);
ssize_t netio_read_until(struct netio *dsc, const char *end, void *buf,
    size_t len, char *last);
ssize_t netio_read(struct netio *dsc, void *buf, size_t len);
int netio_write(struct netio *dsc, const void *data, size_t len);
int netio_printf(struct netio *dsc, const char *fmt, ...);
struct netio *netio_open(int s);
void netio_close(struct netio *dsc);

#endif /* !NETIO_H */
