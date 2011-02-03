MKMK=../../../ben-blinkenlights/ubb/cam/mkmk

NAME=atusb-pgm

# 8.0 + 31 mm
BOARD = X0=39.0mm Y0=8.1mm
FAB = X=0 Y=0 XN=1 YN=2

.PHONY:			all clean

all:			Makefile.mkmk

Makefile.mkmk:		$(MKMK) ../$(NAME).drl ../$(NAME)-PCB_Edges.gbr
			$(MKMK) NAME=$(NAME) $(BOARD) $(FAB)

../$(NAME).drl:		../$(NAME).brd
			pcbnew --drill `pwd`/../$(NAME).brd
			touch $@

../$(NAME)-PCB_Edges.gbr: ../$(NAME).brd
			pcbnew --plot=gerber -l PCB_Edges `pwd`/../$(NAME).brd

-include Makefile.mkmk

clean::
			rm -f Makefile.mkmk
