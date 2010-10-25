#
# atspi/Makefile - Makefile for USB to SPI translator (for AT86RF230)
#
# Written 2010 by Werner Almesberger
# Copyright 2010 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


MAIN = atspi
OBJS = $(MAIN) usb descr version ep0

F32XBASE = ../../../../f32xbase

include $(F32XBASE)/fw/common/Makefile.system
include $(F32XBASE)/fw/common/Makefile.common

CFLAGS += -I../common -I../include
LDFLAGS += --code-size $(PAYLOAD_SIZE) --code-loc $(PAYLOAD_START)

USB_ID = $(shell \
  ( echo '\#include "config.h"'; echo USB_VENDOR:USB_PRODUCT; ) | \
  cpp -I../common -I../include | sed '/^ *$$/d;/^\#/d' )

.PHONY:		dfu

dfu:
		dfu-util -d $(USB_ID) -D $(MAIN).bin
