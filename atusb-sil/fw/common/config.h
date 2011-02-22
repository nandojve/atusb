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
#include "atusb/usb-ids.h"


/* ----- Boot loader configuration ----------------------------------------- */

/*
 * Make LED output push-pull so that we can output a high voltage.
 * This turns on the LED, to indicate that we're in the boot loader.
 */

#define PLATFORM_ENTER \
	LED_MODE |= 1 << LED_BIT;


/*
 * Turn off the LED when we exit the boot loader.
 */

#define PLATFORM_EXIT \
	LED = 0


/* ----- Application configuration ----------------------------------------- */

#if defined(BOARD_100813)
#define	HW_TYPE		HW_TYPE_100813
#elif defined(BOARD_101216)
#define	HW_TYPE		HW_TYPE_101216
#else
#error must define BOARD_100813 or BOARD_101216
#endif

#endif /* !CONFIG_H */
