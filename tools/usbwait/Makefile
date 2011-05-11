#
# usbwait/Makefile - Wait for an USB device to appear
#
# Written 2011 by Werner Almesberger
# Copyright 2011 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


MAIN = usbwait

ifeq ($(TARGET),)
TARGET = host
endif

ifeq ($(TARGET),host)

include ../Makefile.common

else

.PHONY:		all install uninstall clean spotless

all:

install:

uninstall:

clean:
		rm -f $(MAIN).o

spotless:	clean
		rm -f $(MAIN)

endif
