PNGS=flow.png setup-A.png setup-B.png setup-C.png setup-D.png

all:		$(PNGS)

flow.png:	flow.fig
		fig2dev -L png -m 0.7 -S 4 $< $@ || { rm -f $@; exit 1; }

setup-%.png:	setup.fig
		./figfilt `echo $@ | tr -cd 'A-Z'` $< | \
		    fig2dev -L png -m 0.7 -S 4 >$@ || \
		    { rm -f $@; exit 1; }

clean:
		rm -f $(PNGS)
