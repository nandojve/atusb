/*
 * include/daemon.h - Helper functions for proper daemonification
 *
 * Written 2011 by Werner Almesberger
 * Copyright 2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef DAEMON_H
#define DAEMON_H

#include <unistd.h>


pid_t daemonize(void);

#endif /* !DAEMON_H */
