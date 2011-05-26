PNGS=flow.png setup-A.png setup-B.png setup-C.png setup-D.png
JPGS=atusb-programming.jpg

all:		$(PNGS) $(JPGS)

flow.png:	flow.fig
		fig2dev -L png -m 0.7 -S 4 $< $@ || { rm -f $@; exit 1; }

setup-%.png:	setup.fig
		./figfilt `echo $@ | tr -cd 'A-Z'` $< | \
		    fig2dev -L png -m 0.7 -S 4 >$@ || \
		    { rm -f $@; exit 1; }

atusb-programming.jpg:
		wget http://downloads.qi-hardware.com/people/werner/wpan/tmp/atusb-programming.jpg

clean:
		rm -f $(PNGS)
