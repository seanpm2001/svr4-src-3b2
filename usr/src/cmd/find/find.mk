#	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)find:find.mk	1.10.3.3"

#	Makefile for find

ROOT =

DIR = $(ROOT)/usr/bin


INC = $(ROOT)/usr/include
INCSYS = $(ROOT)/usr/include

LDFLAGS = -s -lgen

CFLAGS = -O -I$(INC) -I$(INCSYS)

INS = install

STRIP = strip

SIZE = size

#top#
# Generated by makefile 1.47

MAKEFILE = find.mk


MAINS = find

OBJECTS =  find.o

SOURCES =  find.c

ALL:		$(MAINS)

find:		find.o 
	$(CC) $(CFLAGS) -o find find.o $(LDFLAGS) $(PERFLIBS)


find.o:		 $(INC)/stdio.h $(INC)/errno.h $(INC)/pwd.h \
		 $(INC)/grp.h $(INCSYS)/sys/types.h $(INCSYS)/sys/stat.h \
		 $(INCSYS)/sys/param.h $(INC)/ftw.h $(INC)/limits.h \
		 $(INC)/unistd.h $(INC)/stdlib.h

GLOBALINCS = $(INC)/stdio.h $(INC)/errno.h $(INC)/pwd.h \
	$(INC)/grp.h $(INCSYS)/sys/types.h $(INCSYS)/sys/stat.h \
	$(INCSYS)/sys/param.h $(INC)/ftw.h $(INC)/limits.h \
	$(INC)/unistd.h $(INC)/stdlib.h


clean:
	rm -f $(OBJECTS)

clobber:
	rm -f $(OBJECTS) $(MAINS)

newmakefile:
	makefile -m -f $(MAKEFILE)  -s INC $(INC)
#bottom#

all : ALL

install: ALL
	$(INS) -f $(DIR) -m 0555 -u bin -g bin $(MAINS)

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
