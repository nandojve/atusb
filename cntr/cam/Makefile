DIR=/home/moko/svn.openmoko.org/developers/werner/cncmap

.PHONY:		pcb spool

pcb:
		./doit >job

spool:
		PORT=/dev/ttyUSB0 $(DIR)/spool/spool job
