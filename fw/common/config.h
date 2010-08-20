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


/*
 *  Platform-specific settings
 *
 * USB_VENDOR = Qi Hardware
 * USB_PRODUCT = 802.15.4, device 0
 *                   -- -         -
 */

#define	USB_VENDOR	0x20b7
#define	USB_PRODUCT	0x1540


/* ----- Boot loader configuration ----------------------------------------- */

/*
 * Make LED output push-pull so that we can output a high voltage.
 * This turns on the LED, to indicate that we're in the boot loader.
 */

#define PLATFORM_SETUP \
	LED_MODE |= 1 << LED_BIT;


/*
 * Turn off the LED when we exit the boot loader.
 */

#define PLATFORM_EXIT \
	LED = 0


/* ----- Application configuration ----------------------------------------- */

#define	HW_TYPE		HW_TYPE_100813

#endif /* !CONFIG_H */
