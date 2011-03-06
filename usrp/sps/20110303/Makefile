RUNS=atusb-sil \
  atusb-20110214-3 atusb-20110214-4 atusb-20110214-5 \
  atusb-20110214-6 atusb-20110214-7 \
  atben-20110115-1 \
  atben-20110123-1 atben-20110123-2 \
  atben-20110219-1 atben-20110219-2 atben-20110219-3 \
  atben-20110219-4 atben-20110219-5

all:

upload:
	rsync -e ssh --progress index.html *.png \
	  www-data@downloads.qi-hardware.com:werner/wpan/20110303/

index:
	PATH=..:../..:$$PATH time ../row $(RUNS) >index.html
