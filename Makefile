.PHONY:		all dsv

all:
		@echo "make what ? target: dsv"
		@exit 1

dsv:
		scripts/dsv setup BOOKSHELF
