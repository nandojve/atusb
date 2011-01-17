PLOT_BRD = pcbnew --plot=ps_a4 --ps-pads-drill-opt=none --fill-all-zones

NAME = atben

.PHONY:		all gen generate sch brd xpdf front back clean

all:
		@echo "make what ? target: gen sch brd xpdf"
		@exit 1

gen generate:
		eeschema --plot `pwd`/$(NAME).sch
		# need scripts

sch:
		eeschema `pwd`/$(NAME).sch

brd:
		pcbnew `pwd`/$(NAME).brd

xpdf:
		xpdf $(NAME).pdf

front:		$(NAME)-Front.ps
		lpr $<

back:		$(NAME)-Back.ps
		lpr $<

#
# Postscript for production of front/back layer, using the toner transfer
# method. Note that other artwork transfer methods may require different
# mirror settings.
#
# We use  --ps-pads-drill-opt=none  to avoid having any hole before drilling,
# which yields the best results with a CNC drill. For manual drilling, "real"
# would be preferrable. Do not use "small", for this created holes that are
# larger (!) than designed.
#

%-Front.ps:	%.brd
		$(PLOT_BRD) -l Front --mirror $<

%-Back.ps:	%.brd
		$(PLOT_BRD) -l Back $<

clean:
		rm -f $(NAME)-Front.ps $(NAME)-Back.ps
		rm -f $(NAME).drl $(NAME)-PCB_Edges.gbr
