/*
 * fw/mac.h - HardMAC functions
 *
 * Written 2011 by Werner Almesberger
 * Copyright 2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef MAC_H
#define	MAC_H

#include <stdint.h>


extern void (*mac_irq)(void);

int mac_rx(int on);
int mac_tx(uint16_t flags, uint16_t len);

#endif /* !MAC_H */
