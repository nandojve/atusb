NAME = atusb
CFLAGS = -g -Wall -Wextra -Wshadow -Werror -Wno-unused \
	 -Wmissing-prototypes -Wmissing-declarations -Wstrict-prototypes

CHIP=atmega32u2
CHIP_AVRDUDE=at90usb162	# @@@ fix this when support for the ATmega32U2 becomes
			# available

AVR_PREFIX = $(BIN_PATH) avr-
CC = $(AVR_PREFIX)gcc
OBJCOPY = $(AVR_PREFIX)objcopy
#OBJDUMP = $(AVR_PREFIX)objdump

FreakUSB = usb
USB_OBJS = $(FreakUSB)/usb/usb.o $(FreakUSB)/usb/ctrl.o \
  $(FreakUSB)/usb/usb_buf.o \
  $(FreakUSB)/hw/at90usbxx2/ep.o $(FreakUSB)/hw/at90usbxx2/hw.o \
  $(FreakUSB)/hw/at90usbxx2/isr.o
OBJS = atusb.o board.o spi.o descr.o ep0.o $(USB_OBJS)

CFLAGS += -I../fw/include \
	  -I$(FreakUSB)/usb -I$(FreakUSB)/hw/at90usbxx2 \
	  -DNUM_EPS=1

.PHONY:	all clean upload prog

all:	$(NAME).bin

%.o:		%.c
		$(CC) $(CFLAGS) -mmcu=$(CHIP) -Os -c $<

$(NAME).elf:	$(OBJS)
		$(CC) $(CFLAGS) -mmcu=$(CHIP) -o $@ $(notdir $(OBJS))

%.bin:		%.elf
		$(OBJCOPY) -j .text -j .data -O binary $< $@

clean:
		rm -f $(NAME).bin $(NAME).elf $(notdir $(OBJS))

upload:		$(NAME).bin
		scp $(NAME).bin jlime:

prog:
		ssh jlime avrdude -F -p $(CHIP_AVRDUDE) -c nanonote_atusb -e \
		  -U flash:w:$(NAME).bin:r \
		  -U lfuse:w:0x60:m		# external clock, slow start-up

on:
		ssh jlime poke 0x10010318 4

off:
		ssh jlime poke 0x10010314 4
