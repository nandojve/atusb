#
# boot/Makefile - Makefile for DFU-capable boot loader
#
# Written 2008, 2010 by Werner Almesberger
# Copyright 2008, 2010 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#

MAIN=boot
OBJS=$(MAIN) usb dfu version

F32XBASE = ../../../f32xbase

include $(F32XBASE)/fw/common/Makefile.system
include $(F32XBASE)/fw/common/Makefile.common

CFLAGS += -DLOW_SPEED
LDFLAGS += --code-size $(PAYLOAD_START)

boot.rel:	$(F32XBASE)/fw/boot/boot.c
		$(CC) $(CFLAGS) -o $@ -c $<

usb.rel:	$(F32XBASE)/fw/common/usb.c
		$(CC) $(CFLAGS) -o $@ -c $<

dfu.rel:	$(F32XBASE)/fw/boot/dfu.c
		$(CC) $(CFLAGS) -o $@ -c $<
