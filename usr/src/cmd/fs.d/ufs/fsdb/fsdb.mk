#	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)ufs.cmds:ufs/fsdb/fsdb.mk	1.3"

ROOT=
INC = $(ROOT)/usr/include
TESTDIR = .
INSDIR1 = $(ROOT)/usr/lib/fs/ufs
CFLAGS = -O -I$(INC)
LDFLAGS = -s 
INS=install

all:  install clobber

fsdb: fsdb.c $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o fsdb fsdb.c $(OBJS) $(SHLIBS)

install: fsdb
	@if [ ! -d $(ROOT)/usr/lib/fs ]; \
		then \
		mkdir $(ROOT)/usr/lib/fs; \
		fi;
	@if [ ! -d $(INSDIR1) ]; \
		then \
		mkdir $(INSDIR1); \
		fi;
	$(INS) -f $(INSDIR1) -m 0555 -u bin -g bin $(TESTDIR)/fsdb


clean:
	-rm -f fsdb.o

clobber: clean
	rm -f fsdb
