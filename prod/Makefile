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
SPECTRUM_MARGIN=+5 +5

GENPATHPROF=../tools/atrf-path/genpathprof


.PHONY:	all ben usb flash spectrum spotless

all:	ben

ben:	ben.profile
	./atben $(ATBEN_REF) $(ATBEN_DUT)

usb:	usb.profile
	./atusb $(ATUSB_REF) $(ATUSB_DUT)

flash:
	./atusb-flash $(ATUSB_REF) $(ATUSB_DUT)

spectrum:
	@echo "$(ATBEN_REF) => $(ATBEN_DUT)"
	atrf-path -g $(ATBEN_REF) $(ATBEN_DUT) 10 | \
	  $(GENPATHPROF) $(SPECTRUM_MARGIN) >ben.profile || \
	  { rm -rf ben.profile; exit 1; }
	@echo "$(ATBEN_DUT) => $(ATBEN_BEN)"
	atrf-path -g $(ATUSB_REF) $(ATUSB_DUT) 10 | \
	  $(GENPATHPROF) $(SPECTRUM_MARGIN) >usb.profile || \
	  { rm -rf usb.profile; exit 1; }

ben.profile:
	cp ../tools/atrf-path/profile.example $@

usb.profile:
	cp ../tools/atrf-path/profile.example $@

spotless:
	rm -f ben.profile usb.profile
