PLOT_BRD = pcbnew --plot=ps_a4 --ps-pads-drill-opt=none --fill-all-zones

.PHONY:		all gen generate sch brd xpdf front back clean

all:
		@echo "make what ? target: gen sch brd xpdf front back clean"
		@exit 1

gen generate:
		eeschema --plot `pwd`/atusb.sch
		# need scripts

sch:
		eeschema `pwd`/atusb.sch

brd:
		pcbnew `pwd`/atusb.brd

xpdf:
		xpdf atusb.pdf

front:		atusb-Front.ps
		lpr $<

back:		atusb-Back.ps
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
# Note: all this currenly only works properly if the board has been saved with
# all zones filled.
#

atusb-Front.ps:	atusb.brd
		$(PLOT_BRD) -l Front --mirror $<

atusb-Back.ps:	atusb.brd
		$(PLOT_BRD) -l Back $<

clean:
		rm -f atusb-Front.ps atusb-Back.ps
		rm -f atusb.drl atusb-PCB_Edges.gbr
