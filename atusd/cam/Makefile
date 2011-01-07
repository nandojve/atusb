.PHONY:			all clean

all:			Makefile.mkmk

Makefile.mkmk:		mkmk ../atusd.drl ../atusd-PCB_Edges.gbr
			./mkmk

../atusd.drl:		../atusd.brd
			pcbnew --drill `pwd`/../atusd.brd

../atusd-PCB_Edges.gbr:	../atusd.brd
			pcbnew --plot=gerber -l PCB_Edges `pwd`/../atusd.brd

-include Makefile.mkmk

clean::
			rm -f Makefile.mkmk
