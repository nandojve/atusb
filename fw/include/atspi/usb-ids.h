/*
 * include/atspi/usb-ids.h - USB vendor and product IDs
 *
 * Written 2009, 2010 by Werner Almesberger
 * Copyright 2009, 2010 Werner Almesberger
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
 * USB_PRODUCT = 802.15.4, device 0
 *                   -- -         -
 */

#define USB_VENDOR	0x20b7	/* Qi Hardware */
#define USB_PRODUCT	0x1540	/* ben-wpan, AT86RF230-based */

#endif /* !USB_IDS_H */
