#	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)killall:killall.mk	1.5.1.2"

#	Makefile for killall 

ROOT =

DIR = $(ROOT)/usr/sbin

SYMLINK = :

INC = $(ROOT)/usr/include

LDFLAGS = -s $(SHLIBS)

CFLAGS = -O -I$(INC)

INS = install

STRIP = strip

SIZE = size

#top#
# Generated by makefile 1.47

MAKEFILE = killall.mk


MAINS = killall

OBJECTS =  killall.o

SOURCES =  killall.c

ALL:		$(MAINS)

killall:	killall.o 
	$(CC) $(CFLAGS)  -o killall  killall.o   $(LDFLAGS)

killall.o:	$(INC)/signal.h $(INC)/sys/types.h $(INC)/sys/procset.h  \
			$(INC)/errno.h

GLOBALINCS =	$(INC)/signal.h $(INC)/sys/types.h $(INC)/sys/procset.h  \
			$(INC)/errno.h

clean:
	rm -f $(OBJECTS)

clobber:
	rm -f $(OBJECTS) $(MAINS)

newmakefile:
	makefile -m -f $(MAKEFILE)  -s INC $(INC)
#bottom#

all : ALL

install: ALL
	-rm -f $(ROOT)/etc/killall
	$(INS) -f $(DIR) -m 0555 -u bin -g bin $(MAINS)
	-$(SYMLINK) /usr/sbin/killall $(ROOT)/etc/killall

size: ALL
	$(SIZE) $(MAINS)

strip: ALL
	$(STRIP) $(MAINS)

#	These targets are useful but optional

partslist:
	@echo $(MAKEFILE) $(SOURCES) $(LOCALINCS)  |  tr ' ' '\012'  |  sort

productdir:
	@echo $(DIR) | tr ' ' '\012' | sort

product:
	@echo $(MAINS)  |  tr ' ' '\012'  | \
	sed 's;^;$(DIR)/;'

srcaudit:
	@fileaudit $(MAKEFILE) $(LOCALINCS) $(SOURCES) -o $(OBJECTS) $(MAINS)