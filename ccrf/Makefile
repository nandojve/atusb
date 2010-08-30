.PHONY:		all gen generate sch brd xpdf

all:
		@echo "make what ? target: gen sch brd xpdf"
		@exit 1

gen generate:
		eeschema --plot `pwd`/wpan-ccrf.sch
		# need scripts

sch:
		eeschema `pwd`/wpan-ccrf.sch

brd:
		pcbnew `pwd`/wpan-ccrf.brd

xpdf:
		xpdf wpan-ccrf.pdf
