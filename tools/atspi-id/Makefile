#
# atspi-id/Makefile - Build the ATSPI identification utility
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

include $(F32XBASE)/lib/Makefile.common

CFLAGS += -I../../atrf/fw/include -I../include
LDLIBS += -L../lib -latspi
