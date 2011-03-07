# we need bash for {1,2,3}
SHELL=/bin/bash

TRIES_1=a b c d e f g
TRIES_2=i j
TRIES_3=a b c
RUNS=atusb-20110214-4-0 atusb-20110214-4-1 atusb-20110214-4-2 \
  $(TRIES_1:%=atusb-20110214-4%-{0,1,2}) \
  atusb-20110214-4h-0 atusb-20110214-4h-1 \
  $(TRIES_2:%=atusb-20110214-4%-{0,1,2}) \
  atusb-20110214-3-0 atusb-20110214-3-1 atusb-20110214-3-2 \
  $(TRIES_3:%=atusb-20110214-3%-{0,1,2}) \

all:
	echo $(RUNS)
upload:
	rsync -e ssh --progress index.html *.png \
	  www-data@downloads.qi-hardware.com:werner/wpan/20110306/

index:
	PATH=..:../..:$$PATH time ../row $(RUNS) >index.html
