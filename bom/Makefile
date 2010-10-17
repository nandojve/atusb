BOOM=PATH=/home/moko/svn.openmoko.org/trunk/eda/boom:../boom:$$PATH boom

BOOM2=../../eda-tools/boom
BOOM_CONFIG=$(BOOM2)/boom-config

KITS=1

EQU=atrf.equ dk/digi-key.equ $(shell $(BOOM_CONFIG) equ)
INV=atrf.inv dk/digi-key.inv $(shell $(BOOM_CONFIG) inv)
DSC=dk/digi-key.dsc $(shell $(BOOM_CONFIG) dsc)
CHR=$(shell $(BOOM_CONFIG) chr)


.PHONY:		all again show-atusd spotless

all:		atusd.ord

again:
		$(MAKE) spotless
		$(MAKE) all

atusd.ord:	atusd.par $(INV) $(EQU)
		$(BOOM) part2order $(KITS) $(INV) $(EQU) atusd.par >$@ || \
		  { rm -rf $@; exit 1; }

atusd.par:	$(EQU) $(INV) $(CHR) ../atusd/atusd.lst atrf.sub
		$(BOOM) bom2part $(EQU) $(INV) $(CHR) \
		  ../atusd/atusd.lst atrf.sub \
		  >$@ || { rm -rf $@; exit 1; }

dk/digi-key.inv:
		$(MAKE) -C dk digi-key.inv

dk/digi-key.dsc:
		$(MAKE) -C dk digi-key.dsc

SHOW_PRETTY = (echo '\#ORD'; grep '^$(2) ' $(1).ord ; ) | \
		$(BOOM) prettyord - $(3) | \
		sed 's/^...  //' | \
		awk '{ s += $$(NF); if ($$(NF)+0) n++; print; } \
		END { print "$(4)", s, "items:", n }'

show-atusd:	atusd.ord $(DSC)
		$(call SHOW_PRETTY,atusd,DIGI-KEY,$(DSC),USD)

spotless:
		$(MAKE) -C dk spotless
		rm -f atusd.par atusd.ord
