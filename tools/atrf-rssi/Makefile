#
# atrf-rssi/Makefile - Build the spectrum scan utility
#
# Written 2010-2011 by Werner Almesberger
# Copyright 2010-2011 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


MAIN = atrf-rssi

include ../Makefile.common

CFLAGS_host += $(shell sdl-config --cflags)
MACROS_host += -DHAVE_GFX
LDLIBS_host += $(shell sdl-config --libs) -lSDL_gfx
OBJS_host = gui.o zgrid.o digit.o letter.o

CFLAGS_ben_jlime += $(shell sdl-config --cflags)
MACROS_ben_jlime += -DHAVE_GFX
LDLIBS_ben_jlime += -lSDL -lSDL_gfx
OBJS_ben_jlime = gui.o zgrid.o digit.o letter.o

$(MAIN):	$(OBJS_$(TARGET))
