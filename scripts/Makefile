SHELL = /bin/bash

.PHONY:		all ben-wpan-schhist xue-schhist

# All the targets are for demo purposes pnly !

all:
		@echo "possible targets:" 2>&1
		@echo "  ben-wpan-schhist ben-wpan-schhist-upload" 2>&1
		@echo "  xue-schhist xue-schhist-upload" 2>&1
		@exit 1

ben-wpan-schhist:
	SCHHIST_TITLE=ben-wpan/atrf \
	  SCHHIST_HOME_URL=http://projects.qi-hardware.com/index.php/p/ben-wpan/ \
	  SCHHIST_COMMIT_TEMPLATE='http://projects.qi-hardware.com/index.php/p/ben-wpan/source/commit/{}/' \
	  ./schhist2web atrf/wpan-atrf.sch

ben-wpan-schhist-upload:
	 rsync -a --progress _out/{index.html,unchanged.png,thum*,diff*} \
	   werner@host:/home/httpd/almesberger/misc/ben/demo/

xue-schhist:
	SCHHIST_TITLE=Xue \
	  SCHHIST_HOME_URL=http://projects.qi-hardware.com/index.php/p/xue/ \
	  SCHHIST_COMMIT_TEMPLATE='http://projects.qi-hardware.com/index.php/p/xue/source/commit/{}/' \
	  ./schhist2web -S ../../xue kicad/xue-rnc/xue-rnc.sch _xue

xue-schhist-upload:
	 rsync -a --progress _xue/{index.html,unchanged.png,thum*,diff*} \
	   werner@host:/home/httpd/almesberger/misc/ben/demo2/
