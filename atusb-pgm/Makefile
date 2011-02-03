PLOT_BRD = pcbnew --plot=ps_a4 --ps-pads-drill-opt=none --fill-all-zones

NAME = atusb-pgm

.PHONY:		all sch brd front back clean

all:
		@echo "make what ? target: sch brd front back clean"
		@exit 1

sch:
		eeschema `pwd`/$(NAME).sch

brd:
		pcbnew `pwd`/$(NAME).brd

front:		$(NAME)-Front.ps
		lpr $<

back:		$(NAME)-Back.ps
		lpr $<

%-Front.ps:	%.brd
		$(PLOT_BRD) -l Front --mirror $<

%-Back.ps:	%.brd
		$(PLOT_BRD) -l Back $<

clean:
		rm -f $(NAME)-Front.ps $(NAME)-Back.ps
		rm -f $(NAME).drl $(NAME)-PCB_Edges.gbr
