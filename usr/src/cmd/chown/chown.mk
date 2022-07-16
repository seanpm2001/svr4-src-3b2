#	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)chown:chown.mk	1.4.3.2"

#		PROPRIETARY NOTICE (Combined)
#
#This source code is unpublished proprietary information
#constituting, or derived under license from AT&T's UNIX(r) System V.
#In addition, portions of such source code were derived from Berkeley
#4.3 BSD under license from the Regents of the University of
#California.
#
#
#
#		Copyright Notice 
#
#Notice of copyright on this source code product does not indicate 
#publication.
#
#	(c) 1986,1987,1988,1989  Sun Microsystems, Inc
#	(c) 1983,1984,1985,1986,1987,1988,1989  AT&T.
#	          All rights reserved.


#	Makefile for chown	

ROOT =

DIR = $(ROOT)/usr/bin

INC = $(ROOT)/usr/include
INCSYS = $(ROOT)/usr/include

INS = install

LDFLAGS = -s

CFLAGS = -O -I$(INC) -I$(INCSYS)

STRIP = strip

SIZE = size

#top#
# Generated by makefile 1.47

MAKEFILE = chown.mk


MAINS = chown

OBJECTS =  chown.o

SOURCES =  chown.c

ALL:		$(MAINS)

chown:		chown.o	
	$(CC) $(CFLAGS)  -o chown  chown.o $(LDFLAGS) $(SHLIBS)


chown.o:	 $(INC)/stdio.h $(INC)/ctype.h \
		 $(INCSYS)/sys/types.h $(INCSYS)/sys/stat.h \
		 $(INC)/dirent.h $(INCSYS)/sys/dir.h \
		 $(INC)/pwd.h $(INC)/unistd.h


GLOBALINCS = $(INC)/ctype.h $(INC)/pwd.h $(INC)/stdio.h \
	$(INCSYS)/sys/stat.h $(INCSYS)/sys/types.h $(INCSYS)/sys/dir.h \
	$(INC)/dirent.h $(INC)/unistd.h


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
