/*
 * atspi-txrx/atspi-txrx.c - ben-wpan AT86RF230 TX/RX
 *
 * Written 2010 by Werner Almesberger
 * Copyright 2010 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "at86rf230.h"
#include "atspi.h"
#include "misctxrx.h"


/*
 * According to IEEE 802.15.4-2003 section E.2.6, channel 15 is the only
 * channel that falls into the 802.11 guard bands in North America an Europe.
 */

#define	DEFAULT_CHANNEL	15	/* channel 15, 2425 MHz */

/*
 * Transmit power, dBm. IEEE 802.15.4-2003 section E.3.1.3 specifies a transmit
 * power of 0 dBm for IEEE 802.15.4. We assume an antenna gain of 3 dB or
 * better.
 */

#define	DEFAULT_POWER	-3.2	/* transmit power, dBm */


static double tx_pwr[] = {
	 3.0,	 2.6,	 2.1,	 1.6,
	 1.1,	 0.5,	-0.2,	-1.2,
	-2.2,	-3.2,	-4.2,	-5.2,
	-7.2,	-9.2,	-12.2,	-17.2
};


static struct atspi_dsc *init_txrx(int trim)
{
	struct atspi_dsc *dsc;

	dsc = atspi_open();
	if (!dsc)
		exit(1);
	
	atspi_reset_rf(dsc);
	atspi_reg_write(dsc, REG_TRX_STATE, TRX_CMD_TRX_OFF);
#ifdef HAVE_USB /* @@@ yeah, ugly */
	atspi_reg_write(dsc, REG_XOSC_CTRL,
	    (XTAL_MODE_INT << XTAL_MODE_SHIFT) | trim);
#else
	atspi_reg_write(dsc, REG_XOSC_CTRL, XTAL_MODE_EXT << XTAL_MODE_SHIFT);
#endif
	atspi_reg_write(dsc, REG_TRX_CTRL_0, 0); /* disable CLKM */

	(void) atspi_reg_read(dsc, REG_IRQ_STATUS);

	return dsc;
}


static void set_channel(struct atspi_dsc *dsc, int channel)
{
	atspi_reg_write(dsc, REG_PHY_CC_CCA, (1 << CCA_MODE_SHIFT) | channel);
}


static void set_power(struct atspi_dsc *dsc, double power)
{
	int n;

	for (n = 0; n != sizeof(tx_pwr)/sizeof(*tx_pwr)-1; n++)
		if (tx_pwr[n] <= power)
			break;
	atspi_reg_write(dsc, REG_PHY_TX_PWR, TX_AUTO_CRC_ON | n);
}


static void receive(struct atspi_dsc *dsc)
{
	uint8_t buf[MAX_PSDU+1]; /* PSDU+LQI */
	int n, ok, i;
	uint8_t ed, lqi;

	atspi_reg_write(dsc, REG_TRX_STATE, TRX_CMD_RX_ON);
	/*
	 * 180 us, according to AVR2001 section 4.2. We time out after
	 * nominally 200 us.
	 */
	wait_for_interrupt(dsc, IRQ_PLL_LOCK, IRQ_PLL_LOCK, 10, 20);

	fprintf(stderr, "Ready.\n");
	wait_for_interrupt(dsc, IRQ_TRX_END, IRQ_TRX_END | IRQ_RX_START,
	    10, 0);

	n = atspi_buf_read(dsc, buf, sizeof(buf));
	if (n < 0)
		exit(1);
	if (n < 3) {
		fprintf(stderr, "%d bytes received\n", n);
		exit(1);
	}
	ed = atspi_reg_read(dsc, REG_PHY_ED_LEVEL);
	ok = !!(atspi_reg_read(dsc, REG_PHY_RSSI) & RX_CRC_VALID);
	lqi = buf[n-1];
	fprintf(stderr, "%d bytes payload, CRC %s, LQI %u, ED %d dBm\n",
	    n-3, ok ? "OK" : "BAD", lqi, -91+ed);
	for (i = 0; i != n-3; i++)
		putchar(buf[i] < ' ' || buf[i] > '~' ? '?' : buf[i]);
	putchar('\n');
}


static void transmit(struct atspi_dsc *dsc, const char *msg)
{
	uint8_t buf[MAX_PSDU];

	atspi_reg_write(dsc, REG_TRX_STATE, TRX_CMD_PLL_ON);
	/*
	 * 180 us, according to AVR2001 section 4.3. We time out after
	 * nominally 200 us.
	 */
	wait_for_interrupt(dsc, IRQ_PLL_LOCK, IRQ_PLL_LOCK, 10, 20);

	/*
	 * We need to copy the message to append the CRC placeholders.
	 */
	strcpy((void *) buf, msg);
	atspi_buf_write(dsc, buf, strlen(msg)+2);

	/* @@@ should wait for clear channel */
	atspi_reg_write(dsc, REG_TRX_STATE, TRX_CMD_TX_START);

	/* wait up to 10 ms (nominally) */
	wait_for_interrupt(dsc, IRQ_TRX_END, IRQ_TRX_END | IRQ_PLL_LOCK,
	  10, 1000);
}


static void usage(const char *name)
{
	fprintf(stderr,
"usage: %s [-c channel] [-p power] [-t trim] [message]\n"
"    -c channel  channel number, 11 to 26 (default %d)\n"
"    -p power    transmit power, -17.2 to 3.0 dBm (default %.1f)\n"
"    -t trim     trim capacitor, 0 to 15 (default 0)\n"
	    , name , DEFAULT_CHANNEL, DEFAULT_POWER);
	exit(1);
}


int main(int argc, char *const *argv)
{
	int channel = DEFAULT_CHANNEL;
	double power = DEFAULT_POWER;
	int trim = 0;
	char *end;
	int c;
	struct atspi_dsc *dsc;

	while ((c = getopt(argc, argv, "c:p:t:")) != EOF)
		switch (c) {
		case 'c':
			channel = strtoul(optarg, &end, 0);
			if (*end)
				usage(*argv);
			if (channel < 11 || channel > 26)
				usage(*argv);
			break;
		case 'p':
			power = strtod(optarg, &end);
			if (*end)
				usage(*argv);
			break;
		case 't':
			trim = strtoul(optarg, &end, 0);
			if (*end)
				usage(*argv);
			if (trim > 15)
				usage(*argv);
			break;
		default:
			usage(*argv);
		}

	switch (argc-optind) {
	case 0:
		dsc = init_txrx(trim);
		set_channel(dsc, channel);
		receive(dsc);
		break;
	case 1:
		dsc = init_txrx(trim);
		set_channel(dsc, channel);
		set_power(dsc, power);
		transmit(dsc, argv[optind]);
		break;
	default:
		usage(*argv);
	}

	atspi_close(dsc);

	return 0;
}
