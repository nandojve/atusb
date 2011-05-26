PNGS=flow.png setup-A.png setup-B.png setup-C.png setup-D.png
HTMLS=index.html setup.html flash.html test.html analysis.html
JPGS=atusb-programming.jpg atrf-path.png atrf-path-small.png

.PHONY:		all upload clean

all:		$(PNGS) $(JPGS)

flow.png:	flow.fig
		fig2dev -L png -m 0.7 -S 4 $< $@ || { rm -f $@; exit 1; }

setup-%.png:	setup.fig
		./figfilt `echo $@ | tr -cd 'A-Z'` $< | \
		    fig2dev -L png -m 0.7 -S 4 >$@ || \
		    { rm -f $@; exit 1; }

atusb-programming.jpg:
		wget http://downloads.qi-hardware.com/people/werner/wpan/tmp/atusb-programming.jpg

atrf-path.png:
		wget http://downloads.qi-hardware.com/people/werner/wpan/tmp/atrf-path.png

atrf-path-small.png: \
		  atrf-path.png
		convert -scale 50% $< $@

upload:		$(HTMLS) $(PNGS) $(JPGS)
		rsync -e ssh $^ \
		    www-data@downloads.qi-hardware.com:werner/wpan/prod/

clean:
		rm -f $(PNGS)
