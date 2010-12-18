#
# fw/Makefile - ATUSB firmware build
#
# Written 2008-2010 by Werner Almesberger
# Copyright 2008-2010 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


DIRS=common boot atusb


.PHONY:		all depend install uninstall clean spotless

all:
		for d in $(DIRS); do $(MAKE) -C $$d all || exit 1; done

depend:
		for d in $(DIRS); do $(MAKE) -C $$d depend || exit 1; done

# Top-level Makefile recurses for "install" and "uninstall", which have no use
# here. Just ignore them.

install:

uninstall:

clean:
		for d in $(DIRS); do $(MAKE) -C $$d clean || exit 1; done

spotless:
		for d in $(DIRS); do $(MAKE) -C $$d spotless || exit 1; done
