PLOT_BRD = pcbnew --plot=ps_a4 --ps-pads-drill-opt=none --fill-all-zones

NAME = atusd

.PHONY:		all gen generate sch brd xpdf front back clean

all:		dtc123je.mod
		@echo "make what ? target: gen sch brd xpdf"
		@exit 1

gen generate:
		eeschema --plot `pwd`/atusd.sch
		# need scripts

sch:
		eeschema `pwd`/atusd.sch

brd:
		pcbnew `pwd`/atusd.brd

xpdf:
		xpdf atusd.pdf

front:		atusd-Front.ps
		lpr $<

back:		atusd-Back.ps
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

atusd-Front.ps:	atusd.brd
		$(PLOT_BRD) -l Front --mirror $<

atusd-Back.ps:	atusd.brd
		$(PLOT_BRD) -l Back $<

clean:
		rm -f atusd-Front.ps atusd-Back.ps
		rm -f atusd.drl atusd-PCB_Edges.gbr
