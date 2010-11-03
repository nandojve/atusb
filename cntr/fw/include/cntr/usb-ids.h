/*
 * include/cntr/usb-ids.h - USB vendor and product IDs
 *
 * Written 2010 by Werner Almesberger
 * Copyright 2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef USB_IDS_H
#define USB_IDS_H

/*
 *  Platform-specific settings
 *
 * USB_VENDOR = Qi Hardware
 * USB_PRODUCT = CNTR
 *		 ^ ^^ = C 72, from "leet", http://en.wikipedia.org/wiki/Leet
 *                ^   =  b, because we dont have a suitable replacement
 *			 in leet and B is next to N on the keyboard
 */

#define USB_VENDOR	0x20b7	/* Qi Hardware */
#define USB_PRODUCT	0xcb72	/* Arbitrary-precision frequency counter */

#endif /* !USB_IDS_H */
