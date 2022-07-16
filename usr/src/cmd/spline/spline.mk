#	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#	Portions Copyright (c) 1988, Sun Microsystems, Inc.
#	All Rights Reserved.

#ident	"@(#)spline:spline.mk	1.10"

#	Makefile for spline

ROOT =

DIR = $(ROOT)/usr/bin

INC = $(ROOT)/usr/include

LDFLAGS =

B02 = `if u370; then echo "-b0,2"; fi`

CFLAGS = -O -I$(INC) $(B02)

STRIP = strip

SIZE = size

LIST = lp

INS = install

#top#
# Generated by makefile 1.47

MAKEFILE = spline.mk


MAINS = spline

OBJECTS =  spline.o

SOURCES =  spline.c

ALL:		$(MAINS)

spline:		spline.o 
	$(CC) $(CFLAGS)  -o spline  spline.o   $(LDFLAGS) $(SHLIBS)


spline.o:	 $(INC)/stdio.h $(INC)/math.h

GLOBALINCS = $(INC)/stdio.h $(INC)/math.h


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

listing:
	pr -n $(MAKEFILE) $(SOURCES) | $(LIST)

listmk:
	pr -n $(MAKEFILE) | $(LIST)
