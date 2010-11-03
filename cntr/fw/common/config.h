/*
 * common/config.h - Configuration data for boot loader and application
 *
 * Written 2010 by Werner Almesberger
 * Copyright 2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include "io.h"
#include "io-parts.h"
#include "cntr/usb-ids.h"


/* ----- Boot loader configuration ----------------------------------------- */

/*
 * Disable the watchdog.
 */

#define PLATFORM_SETUP \
	PCA0MD = 0;


/*
 * Make LED output push-pull so that we can output a high voltag, then enable
 * the crossbar. This turns on the LED, to indicate that we're in the boot
 * loader.
 */

#define PLATFORM_ENTER			\
	LEDv1_MODE |= 1 << LEDv1_BIT;	\
	LEDv2_MODE |= 1 << LEDv2_BIT;	\
	XBR1 = XBARE


/*
 * Turn off the LED when we exit the boot loader.
 */

#define PLATFORM_EXIT	\
	LEDv1 = 0;	\
	LEDv2 = 0


/* ----- Application configuration ----------------------------------------- */

#define	HW_TYPE		0

#endif /* !CONFIG_H */
