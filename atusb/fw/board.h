/*
 * fw/board.h - Board-specific functions and definitions
 *
 * Written 2008-2011 by Werner Almesberger
 * Copyright 2008-2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef BOARD_H
#define	BOARD_H

#include <stdint.h>


#define	LED_PORT	B
#define	LED_BIT		  6
#define	nRST_RF_PORT	C
#define	nRST_RF_BIT	  7
#define	SLP_TR_PORT	B
#define	SLP_TR_BIT	  4

#define SCLK_PORT	D
#define SCLK_BIT	  5
#define	MOSI_PORT	D
#define	MOSI_BIT	  3

#define	MISO_PORT	D
#define	MISO_BIT	  2
#define	nSS_PORT	D
#define	nSS_BIT		  1
#define	IRQ_RF_PORT	D
#define	IRQ_RF_BIT	  0


#define	SET_2(p, b)	PORT##p |= 1 << (b)
#define	CLR_2(p, b)	PORT##p &= ~(1 << (b))
#define	IN_2(p, b)	DDR##p &= ~(1 << (b))
#define	OUT_2(p, b)	DDR##p |= 1 << (b)
#define	PIN_2(p, b)	((PIN##p >> (b)) & 1)

#define	SET_1(p, b)	SET_2(p, b)
#define	CLR_1(p, b)	CLR_2(p, b)
#define	IN_1(p, b)	IN_2(p, b)
#define	OUT_1(p, b)	OUT_2(p, b)
#define	PIN_1(p, b)	PIN_2(p, b)

#define	SET(n)		SET_1(n##_PORT, n##_BIT)
#define	CLR(n)		CLR_1(n##_PORT, n##_BIT)
#define	IN(n)		IN_1(n##_PORT, n##_BIT)
#define	OUT(n)		OUT_1(n##_PORT, n##_BIT)
#define	PIN(n)		PIN_1(n##_PORT, n##_BIT)


#define	USB_VENDOR	0x20b7	/* Qi Hardware */
#define	USB_PRODUCT	0x1540	/* ben-wpan atusb */

#define	DFU_USB_VENDOR	USB_VENDOR
#define	DFU_USB_PRODUCT	USB_PRODUCT


#define	HAS_BOARD_SERNUM

extern uint8_t board_sernum[42];


void reset_rf(void);
void reset_cpu(void);
uint8_t read_irq(void);

void led(int on);
void panic(void);

void board_init(void);

#endif /* !BOARD_H */
