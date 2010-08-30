SHELL = /bin/bash

UPLOAD = {index.html,unchanged.png,thumb_*,diff_*,pdf_*,html_*}
DEST = werner@host:/home/httpd/almesberger/misc/ben/
RSYNC = rsync -a --progress

.PHONY:		all ben-wpan-schhist ben-wpan-schhist-upload
.PHONY:		xue-schhist xue-schhist-upload
.PHONY:		cntr-schhist cntr-schhist-upload

# All the targets are for demo purposes pnly !

all:
		@echo "possible targets:" 2>&1
		@echo "  ben-wpan-schhist ben-wpan-schhist-upload" 2>&1
		@echo "  xue-schhist xue-schhist-upload" 2>&1
		@echo "  cntr-schhist cntr-schhist-upload" 2>&1
		@exit 1

ben-wpan-schhist:
	SCHHIST_TITLE=ben-wpan/atrf \
	  SCHHIST_HOME_URL=http://projects.qi-hardware.com/index.php/p/ben-wpan/ \
	  SCHHIST_COMMIT_TEMPLATE='http://projects.qi-hardware.com/index.php/p/ben-wpan/source/commit/{}/' \
	  ./schhist2web atrf/wpan-atrf.sch

ben-wpan-schhist-upload:
	 $(RSYNC) _out/$(UPLOAD) $(DEST)/demo/

xue-schhist:
	SCHHIST_TITLE=Xue \
	  SCHHIST_HOME_URL=http://projects.qi-hardware.com/index.php/p/xue/ \
	  SCHHIST_COMMIT_TEMPLATE='http://projects.qi-hardware.com/index.php/p/xue/source/commit/{}/' \
	  ./schhist2web -S ../../xue kicad/xue-rnc/xue-rnc.sch _xue

xue-schhist-upload:
	 $(RSYNC) _xue/$(UPLOAD) $(DEST)/demo2/

cntr-schhist:
	SCHHIST_TITLE=ben-wpan/cntr \
	  SCHHIST_HOME_URL=http://projects.qi-hardware.com/index.php/p/ben-wpan/ \
	  SCHHIST_COMMIT_TEMPLATE='http://projects.qi-hardware.com/index.php/p/ben-wpan/source/commit/{}/' \
	  ./schhist2web cntr/cntr.sch _cntr

cntr-schhist-upload:
	$(RSYNC) _cntr/$(UPLOAD) $(DEST)/demo3/
