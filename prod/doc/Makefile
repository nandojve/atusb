DOWNLOADED=atusb-programming.jpg atrf-path.png
GENERATED=index.html setup.html flash.html test.html analysis.html \
	  flow.png setup-A.png setup-B.png setup-C.png setup-D.png \
	  atben-A.png atusb-A.png atben-B.png atusb-B.png \
	  atben-A-small.png atusb-A-small.png \
	  atben-B-small.png atusb-B-small.png \
	  atrf-path-small.png
ORIGINAL=atben-clkm.png atusb-clkm.png atusb-clk.png

DL=http://downloads.qi-hardware.com/people/werner/wpan/tmp


.PHONY:		all upload clean spotless

all:		$(DOWNLOADED) $(GENERATED)

%.html:		%.hmac style.inc hmac.pl
		perl hmac.pl -DGEN_DATE=`date -u +'%Y-%m-%d'` $*.hmac >$@ || \
		    { rm -f $@; exit 1; }
		
flow.png:	flow.fig
		fig2dev -L png -m 0.7 -S 4 $< $@ || { rm -f $@; exit 1; }

setup-%.png:	setup.fig
		./figfilt `echo $@ | tr -cd 'A-Z'` $< | \
		    fig2dev -L png -m 0.7 -S 4 >$@ || \
		    { rm -f $@; exit 1; }

%-front.png:
		$(MAKE) -C ../../$* $@
		mv ../../$*/$@ .

atben-%-small.png: \
		  atben.fig
		./figfilt `echo $@ | tr -cd 'A-Z'` $< | \
		    fig2dev -L png -m 0.5 -S 4 >$@ || \
		    { rm -f $@; exit 1; }

atben-%.png:	atben.fig
		./figfilt `echo $@ | tr -cd 'A-Z'` $< | \
		    fig2dev -L png -m 2 -S 4 >$@ || \
		    { rm -f $@; exit 1; }

atusb-%-small.png: \
		  atusb.fig
		./figfilt `echo $@ | tr -cd 'A-Z'` $< | \
		    fig2dev -L png -m 0.5 -S 4 >$@ || \
		    { rm -f $@; exit 1; }

atusb-%.png:	atusb.fig
		./figfilt `echo $@ | tr -cd 'A-Z'` $< | \
		    fig2dev -L png -m 2 -S 4 >$@ || \
		    { rm -f $@; exit 1; }

atusb-programming.jpg:
		wget $(DL)/atusb-programming.jpg

atrf-path.png:
		wget $(DL)/atrf-path.png

atrf-path-small.png: \
		  atrf-path.png
		convert -scale 50% $< $@

upload:		$(GENERATED) $(DOWNLOADED) $(ORIGINAL)
		rsync -e ssh $^ \
		    www-data@downloads.qi-hardware.com:werner/wpan/prod/

clean:
		rm -f $(GENERATED)

spotless:	clean
		rm -f $(DOWNLOADED)
