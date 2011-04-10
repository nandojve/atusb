#
# lib/Makefile - Build the ATRF library
#
# Written 2010-2011 by Werner Almesberger
# Copyright 2010-2011 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


LIB = libatrf.a

include ../Makefile.common

OBJS_host = atusb.o usbopen.o
OBJS_ben_jlime = atben.o
OBJS_ben_openwrt = atben.o

OBJS = atrf.o misctxrx.o cwtest.o netio.o $(OBJS_$(TARGET))

.PHONY:		all clean spotless

all:		$(LIB)

$(LIB):		$(OBJS)
		$(AR) cr $@ $^

clean:
		rm -f $(OBJS)
		rm -f $(OBJS_ben_jlime) $(OBJS_ben_openwrt) $(OBJS_host)

spotless:	clean
		rm -f $(LIB)
