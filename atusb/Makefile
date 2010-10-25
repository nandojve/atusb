.PHONY:		all gen generate sch brd xpdf

all:
		@echo "make what ? target: gen sch brd xpdf"
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
