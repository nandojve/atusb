.PHONY:			all

all:			Makefile.mkmk

Makefile.mkmk:		mkmk ../atusb.drl ../atusb-PCB_Edges.gbr
			./mkmk

../atusb.drl:		../atusb.brd
			pcbnew --drill `pwd`/../atusb.brd

../atusb-PCB_Edges.gbr:	../atusb.brd
			pcbnew --plot=gerber -l PCB_Edges `pwd`/../atusb.brd

-include Makefile.mkmk
