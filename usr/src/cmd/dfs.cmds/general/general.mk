#	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)dfs.cmds:general/general.mk	1.2"
ROOT =
TESTDIR = .
INSDIR = $(ROOT)/usr/sbin
INC = $(ROOT)/usr/include
LDFLAGS = 
INS = install
CFLAGS = -O -s
FRC =

all: general

share: general.c 
	$(CC) -I$(INC) $(CFLAGS) -o $(TESTDIR)/general general.c $(LDFLAGS) $(SHLIBS)

install: all
	$(INS) -f $(INSDIR) -m 0555 $(TESTDIR)/general

clean:
	rm -f general.o

clobber: clean
	rm -f $(TESTDIR)/general
FRC:
