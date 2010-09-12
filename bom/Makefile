BOOM=PATH=/home/moko/svn.openmoko.org/trunk/eda/boom:../boom:$$PATH boom

KITS=1

EQU=atrf.equ dk/digi-key.equ
INV=atrf.inv dk/digi-key.inv
GEN=avx.gen johanson.gen murata.gen tdk.gen yageo-cap.gen
CHR=$(GEN:%.gen=%.chr)


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

avx.chr:	dk/digi-key.equ avx.gen
		$(BOOM) gen2chr AVX dk/digi-key.equ avx.gen >$@ || \
		  { rm -f $@; exit 1; }

johanson.chr:	dk/digi-key.equ johanson.gen
		$(BOOM) gen2chr JOHANSON dk/digi-key.equ johanson.gen >$@ || \
		  { rm -f $@; exit 1; }

murata.chr:	dk/digi-key.equ murata.gen
		$(BOOM) gen2chr MURATA dk/digi-key.equ murata.gen >$@ || \
		  { rm -f $@; exit 1; }

tdk.chr:	dk/digi-key.equ tdk.gen
		$(BOOM) gen2chr TDK dk/digi-key.equ tdk.gen >$@ || \
		  { rm -f $@; exit 1; }

yageo-cap.chr:	dk/digi-key.equ yageo-cap.gen
		$(BOOM) gen2chr YAGEO dk/digi-key.equ yageo-cap.gen >$@ || \
		  { rm -f $@; exit 1; }

dk/digi-key.inv:
		$(MAKE) -C dk digi-key.inv

dk/digi-key.dsc:
		$(MAKE) -C dk digi-key.dsc

SHOW_PRETTY = (echo '\#ORD'; grep '^$(2) ' $(1).ord ; ) | \
		$(BOOM) prettyord - $(3) | \
		sed 's/^...  //' | \
		awk '{ s += $$(NF); if ($$(NF)+0) n++; print; } \
		END { print "$(4)", s, "items:", n }'

show-atusd:	atusd.ord dk/digi-key.dsc
		$(call SHOW_PRETTY,atusd,DIGI-KEY,dk/digi-key.dsc,USD)

spotless:
		$(MAKE) -C dk spotless
		rm -f atusd.par atusd.ord
		rm -f $(GEN:%.gen=%.chr)
