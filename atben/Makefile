PLOT_BRD = pcbnew --plot=ps_a4 --ps-pads-drill-opt=none --fill-all-zones

NAME = atben
VERSION = 110219
DIR = $(NAME)

.PHONY:		all gen generate sch brd xpdf front back clean
.PHONY:		gerber gerbv

all:
		@echo "make what ? target: gen sch brd xpdf"
		@exit 1

gen generate:
		eeschema --plot=ps `pwd`/$(NAME).sch
		# need scripts

sch:
		eeschema `pwd`/$(NAME).sch

brd:
		pcbnew `pwd`/$(NAME).brd

xpdf:
		xpdf $(NAME).pdf

front:		$(NAME)-Front.ps
		lpr $<

back:		$(NAME)-Back.ps
		lpr $<

# --- Industrial production ---------------------------------------------------

PCB_FILES = README-PCB $(NAME)-PCB_Edges.dxf $(NAME).drl \
            $(NAME)-SilkS_Front.gto $(NAME)-Mask_Front.gts \
            $(NAME)-Front.gtl $(NAME)-Back.gbl $(NAME)-Mask_Back.gbs \
            $(NAME)-PCB_Edges.gbr

gerber:
		pcbnew --plot=gerber \
		  -l `pcbnew --list-layers $(NAME).brd | tr '\012' ,` \
		  --fill-all-zones $(NAME).brd \
		  --exclude-pcb-edge

fab:		gerber
		pcbnew --plot=dxf -l PCB_Edges $(NAME).brd
		pcbnew --drill $(NAME).brd
		tar Ccfz .. $(NAME)-pcb-$(VERSION).tar.gz \
		  $(PCB_FILES:%=$(DIR)/%)
		cd ..; zip -l $(DIR)/$(NAME)-pcb-$(VERSION).zip \
		  $(PCB_FILES:%=$(DIR)/%)

gerbv:
		gerbv $(NAME)-Comments.gbr \
		  $(NAME)-SilkS_Front.gto \
		  $(NAME)-SoldP_Front.gtp \
		  $(NAME)-Front.gtl \
		  $(NAME)-Mask_Front.gts \
		  $(NAME)-Back.gbl

upload:
		qippl $(NAME)-pcb-$(VERSION).tar.gz \
		  $(NAME)-pcb-$(VERSION).zip wpan/fab

# --- DIY production (toner transfer) -----------------------------------------

#
# Postscript for production of front/back layer, using the toner transfer
# method. Note that other artwork transfer methods may require different
# mirror settings.
#
# We use  --ps-pads-drill-opt=none  to avoid having any hole before drilling,
# which yields the best results with a CNC drill. For manual drilling, "real"
# would be preferrable. Do not use "small", for this created holes that are
# larger (!) than designed.
#

%-Front.ps:	%.brd
		$(PLOT_BRD) -l Front --mirror $<

%-Back.ps:	%.brd
		$(PLOT_BRD) -l Back $<

# --- Cleanup -----------------------------------------------------------------

clean:
		rm -f $(NAME)-Front.ps $(NAME)-Back.ps
		rm -f $(NAME).drl $(NAME)-PCB_Edges.gbr $(NAME)-PCB_Edges.dxf
		rm -f $(NAME)-Front.gtl $(NAME)-Mask_Front.gts
		rm -f $(NAME)-SilkS_Front.gto $(NAME)-SoldP_Front.gtp
		rm -f $(NAME)-Back.gbl $(NAME)-Mask_Back.gbs
		rm -f $(NAME)-SilkS_Back.gbo $(NAME)-SoldP_Back.gbp
		rm -f $(NAME)-Comments.gbr
