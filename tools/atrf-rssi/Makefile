#
# atspi-rssi/Makefile - Build the spectrum scan utility
#
# Written 2010 by Werner Almesberger
# Copyright 2010 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


MAIN = atspi-rssi

include ../Makefile.common

CFLAGS_host += $(shell sdl-config --cflags)
MACROS_host += -DHAVE_GFX
LDLIBS_host += $(shell sdl-config --libs) -lSDL_gfx
OBJS_host = gui.o zgrid.o digit.o

$(MAIN):	$(OBJS_$(TARGET))
