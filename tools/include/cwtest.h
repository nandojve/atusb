/*
 * include/cwtest.h - AT86RF230/231 constant wave test mode
 *
 * Written 2011 by Werner Almesberger
 * Copyright 2011 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#ifndef CWTEST_H
#define	CWTEST_H

#include <stdint.h>

#include "atrf.h"


void cw_test_begin(struct atrf_dsc *dsc, uint8_t cont_tx);
void cw_test_end(struct atrf_dsc *dsc);

#endif /* !CW_TEST_H */
