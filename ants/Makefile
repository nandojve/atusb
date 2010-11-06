MODULES=meander

.PHONY:         all brd clean

all:		meander.mod

sch:
		eeschema `pwd`/ants.sch

brd:
		pcbnew `pwd`/ants.brd

clean:
		rm -f *~*.fpd
		rm -f meander.mod

meander.mod:	meander.fpd
		fped -k $< $@
