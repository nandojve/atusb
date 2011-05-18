/*
 * lib/daemon.c - Helper functions for proper daemonification
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

#include "daemon.h"


pid_t daemonize(void)
{
	pid_t pid;
	int i;

	pid = fork();
	if (pid < 0) {
		perror("fork");
		exit(1);
	}
	if (pid)
		return pid;
	if (setsid() < 0) {
		perror("setsid");
		exit(1);
	}
	for (i = 0; i <= 2; i++)
		(void) close(i);
	(void) chdir("/");
	return 0;
}
