#
# atspi/Makefile - Build the ATSPI identification utility
#
# Written 2010 by Werner Almesberger
# Copyright 2010 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


F32XBASE = ../../../f32xbase

MAIN = atspi-id
OBJS = $(F32XBASE)/lib/usb.o

include $(F32XBASE)/lib/Makefile.common

CFLAGS += -I$(F32XBASE)/include -I../../fw/include
