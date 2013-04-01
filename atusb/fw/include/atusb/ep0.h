/*
 * include/atusb/ep0.h - EP0 extension protocol
 *
 * Written 2008-2011, 2013 by Werner Almesberger
 * Copyright 2008-2011, 2013 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef EP0_H
#define EP0_H

#include <atusb/atusb.h>


/*
 * EP0 protocol:
 *
 * 0.0	initial release
 * 0.1  addition of ATUSB_TEST
 */

#define EP0ATUSB_MAJOR	0	/* EP0 protocol, major revision */
#define EP0ATUSB_MINOR	1	/* EP0 protocol, minor revision */

#define	HW_TYPE_100813	0	/* 2010-08-13 */
#define	HW_TYPE_101216	1	/* 2010-12-16 */
#define	HW_TYPE_110131	2	/* 2011-01-31, ATmega32U2-based */


/*
 * bmRequestType:
 *
 * D7 D6..5 D4...0
 * |  |     |
 * direction (0 = host->dev)
 *    type (2 = vendor)
 *          recipient (0 = device)
 */

#ifndef USB_TYPE_VENDOR
#define	USB_TYPE_VENDOR		0x40
#endif

#define	USB_DIR_IN		0x80
#define	USB_DIR_OUT		0x00

#define	ATUSB_FROM_DEV(req)	(ATUSB_REQ_FROM_DEV | (req) << 8)
#define	ATUSB_TO_DEV(req)	(ATUSB_REQ_TO_DEV | (req) << 8)


void ep0_init(void);

#endif /* !EP0_H */
