#	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)ufs.cmds:ufs/df/df.mk	1.4"
TESTDIR = .
INSDIR1 = $(ROOT)/usr/lib/fs/ufs
INSDIR2 = $(ROOT)/etc/fs/ufs
INS = install
CFLAGS = -O -I$(INC)
LDFLAGS = -s
INC = $(ROOT)/usr/include

all:  install clobber

df: df.c $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o df df.c $(OBJS) $(ROOTLIBS)

install: df
	@if [ ! -d $(ROOT)/usr/lib/fs ]; \
		then \
		mkdir $(ROOT)/usr/lib/fs; \
		fi;
	@if [ ! -d $(INSDIR1) ]; \
		then \
		mkdir $(INSDIR1); \
		fi;
	@if [ ! -d $(ROOT)/etc/fs ]; \
		then \
		mkdir $(ROOT)/etc/fs; \
		fi;
	@if [ ! -d $(INSDIR2) ]; \
		then \
		mkdir $(INSDIR2); \
		fi;
	$(INS) -f $(INSDIR1) -m 0555 -u bin -g bin $(TESTDIR)/df
	$(INS) -f $(INSDIR2) -m 0555 -u bin -g bin $(TESTDIR)/df


clean:
	-rm -f df.o

clobber: clean
	rm -f df