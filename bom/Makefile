BOOM=PATH=/home/moko/svn.openmoko.org/trunk/eda/boom:../boom:$$PATH boom

BOOM2=../../eda-tools/boom
BOOM_CONFIG=$(BOOM2)/boom-config

KITS=1

EQU=atrf.equ dk/digi-key.equ $(shell $(BOOM_CONFIG) equ)
INV=atrf.inv dk/digi-key.inv $(shell $(BOOM_CONFIG) inv)
DSC=dk/digi-key.dsc $(shell $(BOOM_CONFIG) dsc)
CHR=atrf.chr $(shell $(BOOM_CONFIG) chr)


.PHONY:		all again spotless
.PRECIOUS:	%.par

all:		atusb.ord atusd.ord cntr.ord

again:
		$(MAKE) spotless
		$(MAKE) all

%.ord:		%.par $(INV) $(EQU)
		$(BOOM) part2order $(KITS) $^ >$@ || { rm -f $@; exit 1; }

vpath atusb.lst ../atusb
vpath atusd.lst ../atusd
vpath cntr.lst ../cntr

%.par:		$(EQU) $(INV) $(CHR) %.lst atrf.sub
		$(BOOM) bom2part $^ >$@ || { rm -f $@; exit 1; }

dk/digi-key.inv:
		$(MAKE) -C dk digi-key.inv

dk/digi-key.dsc:
		$(MAKE) -C dk digi-key.dsc

show-%:		%.ord $(DSC)
		grep -vw ATRF $< | $(BOOM) prettyord -t - $(DSC) | \
		  sed 's/^...  //'

spotless:
		$(MAKE) -C dk spotless
		rm -f atusb.par arusb.ord atusd.par atusd.ord cntr.par cntr.ord
