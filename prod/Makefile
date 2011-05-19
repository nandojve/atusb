#
# This Makefile just contains a few convenience commands, for development
#


.PHONY:	all ben usb

all:	ben

ben:	ben.profile
	./atben usb net:ben

usb:	usb.profile
	./atusb net:ben usb

ben.profile:
	cp ../tools/atrf-path/profile.example $@

usb.profile:
	cp ../tools/atrf-path/profile.example $@
