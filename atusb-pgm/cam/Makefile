MKMK=../../../cae-tools/cameo/templates/mkmk-simple
#MKMK=../../../ben-blinkenlights/ubb/cam/mkmk


NAME=atusb-pgm

# 6.8 / 8.4 mm
# + 0 / 59 mm
BOARD = X0=6.8mm Y0=67.4mm
TOOLS = DRILL=7mil,15mil MILL_IN=25mil MILL_OUT=45mil
FAB = X=0 Y=0 XN=3 YN=1

.PHONY:			all clean

all:			Makefile.mkmk

Makefile.mkmk:		$(MKMK) ../$(NAME).drl ../$(NAME)-PCB_Edges.gbr
			$(MKMK) NAME=$(NAME) $(BOARD) $(TOOLS) $(FAB)

../$(NAME).drl:		../$(NAME).brd
			pcbnew --drill `pwd`/../$(NAME).brd
			touch $@

../$(NAME)-PCB_Edges.gbr: ../$(NAME).brd
			pcbnew --plot=gerber -l PCB_Edges `pwd`/../$(NAME).brd

-include Makefile.mkmk

clean::
			rm -f Makefile.mkmk
