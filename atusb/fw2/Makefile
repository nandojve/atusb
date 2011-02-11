NAME = atusb

CFLAGS = -g -Wall -Wextra -Wshadow -Werror -Wno-unused \
	 -Wmissing-prototypes -Wmissing-declarations -Wstrict-prototypes

CHIP=atmega32u2

AVR_PREFIX = $(BIN_PATH) avr-
CC = $(AVR_PREFIX)gcc
OBJCOPY = $(AVR_PREFIX)objcopy
#OBJDUMP = $(AVR_PREFIX)objdump

FreakUSB = usb
USB_OBJS  = usb.o ctrl.o usb_buf.o ep.o hw.o isr.o
OBJS = atusb.o board.o spi.o descr.o ep0.o $(USB_OBJS)

vpath %.c $(FreakUSB)/usb/
vpath %.c $(FreakUSB)/hw/at90usbxx2/

CFLAGS += -I../fw/include \
	  -I$(FreakUSB)/usb -I$(FreakUSB)/hw/at90usbxx2 \
	  -DNUM_EPS=1


.PHONY:	all clean upload prog

all:	$(NAME).bin

%.o:		%.c
		$(CC) $(CFLAGS) -mmcu=$(CHIP) -Os -c $<

$(NAME).elf:	$(OBJS)
		$(CC) $(CFLAGS) -mmcu=$(CHIP) -o $@ $(OBJS)

%.bin:		%.elf
		$(OBJCOPY) -j .text -j .data -O binary $< $@

clean:
		rm -f $(NAME).bin $(NAME).elf $(OBJS)

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
