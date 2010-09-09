#
# tools/Makefile - Build the ATSPI-specific tools
#
# Written 2010 by Werner Almesberger
# Copyright 2010 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


DIRS=atspi-id atspi-reset atspi-rssi atspi-trim atspi-txrx
TARGET_ONLY_DIRS=lib

include ../Makefile.recurse

.PHONY:		upload

upload:		
		scp `for n in $(DIRS); do echo $$n/$$n; done` ben:
