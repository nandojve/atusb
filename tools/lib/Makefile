#
# lib/Makefile - Build the ATSPI library
#
# Written 2010 by Werner Almesberger
# Copyright 2010 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


LIB = libatspi.a

include ../Makefile.common

CFLAGS += -Wall -I$(F32XBASE)/include
OBJS_host = atusb.o
OBJS_ben = atusd.o
OBJS = $(F32XBASE)/lib/usb.o atspi.o $(OBJS_$(TARGET))

.PHONY:		all clean spotless

all:		$(LIB)

$(LIB):		$(OBJS)
		$(AR) cr $@ $^

clean:
		rm -f $(OBJS)

spotless:	clean
		rm -f $(LIB)
