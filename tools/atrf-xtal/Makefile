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


ifeq ($(TARGET),)
TARGET = host
endif

ifeq ($(TARGET),host)

.PHONY:		all install uninstall clean spotless

all:

install:

uninstall:

clean:
		rm -f atben.o atrf-xtal.o

spotless:	clean
		rm -f atrf-xtal

else

MAIN = atrf-xtal

include ../Makefile.common

LDLIBS += -lm
CFLAGS += -O9

OBJS_$(TARGET) = atben.o

$(MAIN):        $(OBJS_$(TARGET))

endif
