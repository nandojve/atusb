#
# atrf-xtal/Makefile - Build the crystal diagnostic utility
#
# Written 2010, 2011 by Werner Almesberger
# Copyright 2010, 2011 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


MAIN = atrf-xtal

include ../Makefile.common

LDLIBS += -lm

MACROS_host += -DHAVE_ATUSB
OBJS_host = atusb.o
LDLIBS_host += -lgsl -lgslcblas

CFLAGS_ben_openwrt += -O9
MACROS_ben_openwrt += -DHAVE_ATBEN
OBJS_ben_openwrt = atben.o

CFLAGS_ben_jlime += -O9
MACROS_ben_jlime += -DHAVE_ATBEN
OBJS_ben_jlime = atben.o


$(MAIN):        $(OBJS_$(TARGET))

clean::
		rm -f $(OBJS_host) $(OBJS_ben_openwrt) $(OBJS_ben_jlime)
