/*
 * include/atspi/ep0.h - EP0 extension protocol
 *
 * Written 2008-2010 by Werner Almesberger
 * Copyright 2008-2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef EP0_H
#define EP0_H

/*
 * Direction	bRequest		wValue		wIndex	wLength
 *
 * ->host	ATSPI_ID		-		-	3
 * ->host	ATSPI_BUILD		-		-	#bytes
 * host->	ATSPI_RESET		-		-	0
 *
 * host->	ATSPI_RF_RESET		-		-	0
 * ->host	ATSPI_POLL_INT		-		-	1
 *
 * host->	ATSPI_REG_WRITE		value		addr	0
 * ->host	ATSPI_REG_READ		-		addr	1
 * host->	ATSPI_BUF_WRITE		-		-	#bytes
 * ->host	ATSPI_BUF_READ		-		-	#bytes
 * host->	ATSPI_SRAM_WRITE	-		addr	#bytes
 * ->host	ATSPI_SRAM_READ		-		addr	#bytes
 */

/*
 * EP0 protocol:
 *
 * 0.0	initial release
 */

#define EP0ATSPI_MAJOR	0	/* EP0 protocol, major revision */
#define EP0ATSPI_MINOR	0	/* EP0 protocol, minor revision */

#define	HW_TYPE_100813	0	/* 100813 */


/*
 * bmRequestType:
 *
 * D7 D6..5 D4...0
 * |  |     |
 * direction (0 = host->dev)
 *    type (2 = vendor)
 *          recipient (0 = device)
 */


#define	ATSPI_TO_DEV(req)	(0x40 | (req) << 8)
#define	ATSPI_FROM_DEV(req)	(0xc0 | (req) << 8)


enum atspi_requests {
	ATSPI_ID			= 0x00,
	ATSPI_BUILD,
	ATSPI_RESET,
	ATSPI_RF_RESET			= 0x10,
	ATSPI_POLL_INT,
	ATSPI_REG_WRITE			= 0x20,
	ATSPI_REG_READ,
	ATSPI_BUF_WRITE,
	ATSPI_BUF_READ,
	ATSPI_SRAM_WRITE,
	ATSPI_SRAM_READ,
};


void ep0_init(void);

#endif /* !EP0_H */
