.PHONY:		all dsv

all:
		@echo "make what ? target: dsv"
		@exit 1

dsv:
		../eda-tools/dsv/dsv setup BOOKSHELF
