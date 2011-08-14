MODULES=0805-6 meander mmcx-f-edge \
	u-fl-receptacle xtal-4

.SUFFIXES:	.fpd .mod
.PHONY:		all clean

all:		$(MODULES:%=%.mod)

clean:
		rm -f *~*.fpd
		rm -f $(MODULES:%=%.mod)

.fpd.mod:
		fped -k $< $@
