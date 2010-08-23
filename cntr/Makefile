.PHONY:		all gen generate sch brd xpdf

all:
		@echo "make what ? target: gen sch brd xpdf"
		@exit 1

gen generate:
		eeschema --plot `pwd`/cntr.sch
		# need scripts

sch:
		eeschema `pwd`/cntr.sch

brd:
		pcbnew `pwd`/cntr.brd

xpdf:
		xpdf cntr.pdf
