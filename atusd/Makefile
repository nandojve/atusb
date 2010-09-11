NAME = atusd

.PHONY:		all gen generate sch brd xpdf

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
