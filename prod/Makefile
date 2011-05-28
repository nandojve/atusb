#
# prod/Makefile - Run the production test process (reference/development)
#
# Written 2011 by Werner Almesberger
# Copyright 2011 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


ATBEN_DUT=net:ben
ATBEN_REF=usb
ATUSB_DUT=usb
ATUSB_REF=net:ben


.PHONY:	all ben usb flash

all:	ben

ben:	ben.profile
	./atben $(ATBEN_REF) $(ATBEN_DUT)

usb:	usb.profile
	./atusb $(ATUSB_REF) $(ATUSB_DUT)

flash:
	./atusb-flash $(ATUSB_REF) $(ATUSB_DUT)

ben.profile:
	cp ../tools/atrf-path/profile.example $@

usb.profile:
	cp ../tools/atrf-path/profile.example $@
