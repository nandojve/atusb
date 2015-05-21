DL=800px-Atben_atusb_prod_03.jpg 800px-Atben_atusb_prod_05.jpg \
   785px-Atben_atusb_prod_09.jpg 800px-Atben_atusb_prod_07.jpg \
   800px-Atben_atusb_prod_10.jpg

GEN=atben-pcba-small.jpg atusb-pcba-small.jpg \
    atusb-use-small.jpg atben-use-small.jpg

all:	$(GEN)

upload:	index.html zoom.html $(GEN) $(DL)
	rsync -e ssh index.html zoom.html releases $(DL) $(GEN) \
	  www-data@downloads.qi-hardware.com:werner/wpan/web/

800px-Atben_atusb_prod_03.jpg:
	wget http://en.qi-hardware.com/w/images/thumb/0/06/Atben_atusb_prod_03.jpg/800px-Atben_atusb_prod_03.jpg

800px-Atben_atusb_prod_05.jpg:
	wget http://en.qi-hardware.com/w/images/thumb/c/ce/Atben_atusb_prod_05.jpg/800px-Atben_atusb_prod_05.jpg

785px-Atben_atusb_prod_09.jpg:
	wget http://en.qi-hardware.com/w/images/thumb/e/e2/Atben_atusb_prod_09.jpg/785px-Atben_atusb_prod_09.jpg

800px-Atben_atusb_prod_07.jpg:
	wget http://en.qi-hardware.com/w/images/thumb/4/4c/Atben_atusb_prod_07.jpg/800px-Atben_atusb_prod_07.jpg

800px-Atben_atusb_prod_10.jpg:
	wget http://en.qi-hardware.com/w/images/thumb/8/8b/Atben_atusb_prod_10.jpg/800px-Atben_atusb_prod_10.jpg

atben-pcba-small.jpg:	800px-Atben_atusb_prod_03.jpg
	convert $< -scale 180x $@

atusb-pcba-small.jpg:	800px-Atben_atusb_prod_05.jpg
	convert $< -sample 100x105%\! -rotate -45 -scale 200x $@

atusb-use-small.jpg:	785px-Atben_atusb_prod_09.jpg
	convert $< -scale 200x $@

atben-use-small.jpg:	800px-Atben_atusb_prod_07.jpg
	convert $< -scale 200x $@

clean:
	rm -f $(GEN)
