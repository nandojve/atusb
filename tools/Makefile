#
# tools/Makefile - Build the ATRF-specific tools
#
# Written 2010-2011 by Werner Almesberger
# Copyright 2010-2011 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


BEN_DIRS=atrf-gpio atrf-id atrf-path atrf-proxy atrf-reset \
	 atrf-rssi atrf-trim atrf-txrx atrf-xmit atrf-xtal
DIRS=$(BEN_DIRS) usbwait
TARGET_ONLY_DIRS=lib

ifeq ($(TARGET),ben_jlime)
HOST = jlime
else
HOST = ben
endif


include ../makefiles/Makefile.recurse

.PHONY:		upload

upload:		
		scp `for n in $(BEN_DIRS); do \
		    echo $$n/$$n; done` $(HOST):/usr/bin/
