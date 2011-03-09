/*
 * fw/flash.c - Board-specific flash functions
 *
 * Written 2011 by Werner Almesberger
 * Copyright 2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdint.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#define F_CPU   8000000UL
#include <util/delay.h>

#include "dfu.h"
#include "board.h"



void flash_start(void)
{
}


int flash_can_write(uint16_t size)
{
	return 0;
}


void flash_write(const uint8_t *buf, uint16_t size)
{
}


uint16_t flash_read(uint8_t *buf, uint16_t size)
{
	return 0;
}
