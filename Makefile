.PHONY:		all gen generate sch brd xpdf dsv

all:
		@echo "make what ? target: gen sch brd xpdf dsv"
		@exit 1

gen generate:
		eeschema --plot `pwd`/wpan-atrf.sch
		# need scripts

sch:
		eeschema `pwd`/wpan-atrf.sch

brd:
		pcbnew `pwd`/wpan-atrf.brd

xpdf:
		xpdf wpan-atrf.pdf

dsv:
		scripts/dsv setup BOOKSHELF
