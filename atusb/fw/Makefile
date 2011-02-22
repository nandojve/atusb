#
# Makefile - Makefile of the ATUSB firmware 
#
# Written 2010-2011 by Werner Almesberger
# Copyright 2010-2011 by Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#

SHELL = /bin/bash

NAME = atusb

CFLAGS = -g -Wall -Wextra -Wshadow -Werror -Wno-unused-parameter \
	 -Wmissing-prototypes -Wmissing-declarations -Wstrict-prototypes

CHIP=atmega32u2

AVR_PREFIX = $(BIN_PATH) avr-
CC = $(AVR_PREFIX)gcc
OBJCOPY = $(AVR_PREFIX)objcopy
#OBJDUMP = $(AVR_PREFIX)objdump

USB_OBJS  = usb.o atu2.o
OBJS = atusb.o board.o spi.o descr.o ep0.o $(USB_OBJS)

vpath %.c usb/

CFLAGS += -Iinclude -Iusb

# ----- Verbosity control -----------------------------------------------------

CC_normal	:= $(CC)
BUILD_normal	:=
DEPEND_normal	:= $(CPP) $(CFLAGS) -MM -MG

CC_quiet	= @echo "  CC       " $@ && $(CC_normal)
BUILD_quiet	= @echo "  BUILD    " $@ && $(BUILD_normal)
DEPEND_quiet	= @$(DEPEND_normal)

ifeq ($(V),1)
    CC		= $(CC_normal)
    BUILD	= $(BUILD_normal)
    DEPEND	= $(DEPEND_normal)
else
    CC		= $(CC_quiet)
    BUILD	= $(BUILD_quiet)
    DEPEND	= $(DEPEND_quiet)
endif

# ----- Rules -----------------------------------------------------------------

.PHONY:	all clean upload prog version.c

all:		$(NAME).bin

$(NAME).elf:	$(OBJS)
		$(MAKE) version.o
		$(CC) $(CFLAGS) -mmcu=$(CHIP) -o $@ $(OBJS) version.o

%.bin:		%.elf
		$(BUILD) $(OBJCOPY) -j .text -j .data -O binary $< $@
		@echo "build #`cat .version`, `ls -l $@`"

# ----- Cleanup ---------------------------------------------------------------

clean:
		rm -f $(NAME).bin $(NAME).elf $(OBJS) $(OBJS:.o=.d)
		rm -f version.c version.d version.o

# ----- Build version ---------------------------------------------------------

version.c:
		@if [ -f .version ]; then \
		    v=`cat .version`; \
		    expr $$v + 1 >.version; \
		else \
		    echo 0 >.version; \
		fi
		@[ -s .version ] || echo 0 >.version
		@echo '/* MACHINE-GENERATED. DO NOT EDIT ! */' >version.c
		@echo '#include "version.h"' >>version.c
		@echo "const char *build_date = \"`date`\";" >>version.c
		@echo "const uint16_t build_number = `cat .version`;" \
		  >>version.c

# ----- Dependencies ----------------------------------------------------------

%.o:            %.c
		$(CC) $(CFLAGS) -mmcu=$(CHIP) -Os -c $<
		$(DEPEND) $< | \
		  sed -e \
		    '/^\(.*:\)\? */{p;s///;s/ *\\\?$$/ /;s/  */:\n/g;H;}' \
		    -e '$${g;p;}' -e d >$*.d; \
		  [ "$${PIPESTATUS[*]}" = "0 0" ] || { rm -f $*.d; exit 1; }

-include $(OBJS:.o=.d)

# ----- Programming and device control ----------------------------------------

upload:		$(NAME).bin
		scp $(NAME).bin jlime:

prog:
		ssh jlime avrdude -F -p $(CHIP) -c nanonote_atusb -e \
		  -U flash:w:$(NAME).bin:r \
		  -U lfuse:w:0x60:m		# external clock, slow start-up

on:
		ssh jlime poke 0x10010318 4

off:
		ssh jlime poke 0x10010314 4

reset:
		ssh jlime poke 0x10010318 2048
		ssh jlime poke 0x10010314 2048
