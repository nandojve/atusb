#
# This Makefile just contains a few convenience commands, for development
#


.PHONY:	all ben usb flash

all:	ben

ben:	ben.profile
	./atben usb net:ben

usb:	usb.profile
	./atusb net:ben usb

flash:
	./atusb-flash net:ben usb

ben.profile:
	cp ../tools/atrf-path/profile.example $@

usb.profile:
	cp ../tools/atrf-path/profile.example $@
