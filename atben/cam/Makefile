NAME=atben

.PHONY:			all clean

all:			Makefile.mkmk

Makefile.mkmk:		mkmk ../$(NAME).drl ../$(NAME)-PCB_Edges.gbr
			./mkmk

../$(NAME).drl:		../$(NAME).brd
			pcbnew --drill `pwd`/../$(NAME).brd

../$(NAME)-PCB_Edges.gbr: ../$(NAME).brd
			pcbnew --plot=gerber -l PCB_Edges `pwd`/../$(NAME).brd

-include Makefile.mkmk

clean::
			rm -f Makefile.mkmk
