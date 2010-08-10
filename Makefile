.PHONY:		all gen generate sch brd xpdf

all:
		@echo "make what ? target: gen sch xpdf brd"
		@exit 1

gen generate:
		eeschema --plot `pwd`/wpan-atrf.sch
		# need scripts

xpdf:
		xpdf wpan-atrf.pdf

sch:
		eeschema `pwd`/wpan-atrf.sch

brd:
		pcbnew `pwd`/wpan-atrf.brd
