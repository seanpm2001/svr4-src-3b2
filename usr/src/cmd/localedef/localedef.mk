#	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)localedef:localedef.mk	1.2"

#	Makefile for vi

ROOT =

DIR = $(ROOT)/usr/bin

INC = $(ROOT)/usr/include

LDFLAGS =

CFLAGS = -O -I$(INC)

INS = install

STRIP = strip

SIZE = size

MAKEFILE = vi.mk

ALL : all

all : 
	cd chrtbl; $(MAKE) -f chrtbl.mk ROOT=$(ROOT) all; cd ..;
	cd colltbl; $(MAKE) -f colltbl.mk ROOT=$(ROOT) all; cd ..;
	cd montbl; $(MAKE) -f montbl.mk ROOT=$(ROOT) all; cd ..

install: all
	if [ ! -d $(ROOT)/usr/lib/locale/C ]; \
	then \
		mkdir -p $(ROOT)/usr/lib/locale/C ;\
		$(CH)mkdir /usr/lib/locale/C/LC_MESSAGES ;\
	fi
	cd chrtbl; $(MAKE) -f chrtbl.mk ROOT=$(ROOT) DIR=$(DIR) install ; cd ..;
	cd colltbl; $(MAKE) -f colltbl.mk ROOT=$(ROOT) DIR=$(DIR) install ; cd ..;
	cd datetbl; $(MAKE) -f datetbl.mk ROOT=$(ROOT) DIR=$(DIR) install ; cd ..;
	cd montbl; $(MAKE) -f montbl.mk ROOT=$(ROOT) DIR=$(DIR) install ; cd ..

#
# Cleanup procedures
#
clobber: 
	cd chrtbl; $(MAKE) -f chrtbl.mk ROOT=$(ROOT) DIR=$(DIR) clobber ; cd ..;
	cd colltbl; $(MAKE) -f colltbl.mk ROOT=$(ROOT) DIR=$(DIR) clobber ; cd ..;
	cd montbl; $(MAKE) -f montbl.mk ROOT=$(ROOT) DIR=$(DIR) clobber ; cd ..

clean:
	cd chrtbl; $(MAKE) -f chrtbl.mk ROOT=$(ROOT) DIR=$(DIR) clean ; cd ..;
	cd colltbl; $(MAKE) -f colltbl.mk ROOT=$(ROOT) DIR=$(DIR) clean ; cd ..;
	cd montbl; $(MAKE) -f montbl.mk ROOT=$(ROOT) DIR=$(DIR) clean ; cd ..
