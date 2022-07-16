#	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)devmgmt:listdgrp/listdgrp.mk	1.9"

INC=$(ROOT)/usr/include
INCSYS=$(ROOT)/usr/include/sys
LIB=$(ROOT)/lib
USRLIB=$(ROOT)/usr/lib
LIBS=$(USRLIB)/libadm.a
BIN=$(ROOT)/usr/bin
INSTALL=install
HDRS=$(INC)/stdio.h $(INC)/string.h $(INC)/devmgmt.h
LCLHDRS=
FILE=listdgrp
PROTO=../Prototype
OBJECTS=listdgrp
SRC=main.c
OBJ=$(SRC:.c=.o)
CFLAGS=-I . -I $(INC) -I $(INCSYS) -ladm $(CCFLAGS)
LDFLAGS=$(LDFLAGS)

all		: $(FILE) 

install: all
	@eval `sed -e '/^![^=]*=/!d' -e 's/^!//' $(PROTO)` ;\
	mkpath() { \
		while true ;\
		do \
			tmpdir=$$1 ;\
			[ -d $$tmpdir ] && break ;\
			while [ ! -d $$tmpdir ] ;\
			do \
				lastdir=$$tmpdir ;\
				tmpdir=`dirname $$tmpdir` ;\
			done ;\
			mkdir $$lastdir ;\
		done ;\
	} ;\
	for object in $(OBJECTS) ;\
	do \
		if entry=`grep "[ 	/]$$object[= 	]" $(PROTO)` ;\
		then \
			set -- $$entry ;\
			path=`eval echo $$3` ;\
			expr $$path : '[^/]' >/dev/null && \
				path=$(BASEDIR)/$$path ;\
			dir=$(ROOT)`dirname $$path` ;\
			[ ! -d $$dir ] && mkpath $$dir ;\
			$(INSTALL) -f $$dir -m $$4 -u $$5 -g $$6 $$object ;\
		else \
			echo "unable to install $$object" ;\
		fi ;\
	done

clobber		: clean
		rm -f $(FILE)

clean		:
		rm -f $(OBJ)

strip		:
		$(CC) -s $(FILE).o -o $(FILE) $(LDLIBPATH) $(CFLAGS)

lintit		:
		for i in $(SRC); \
		do \
		    lint $(CFLAGS) $$i; \
		done

$(FILE)		: $(OBJ) $(LIBS)
		$(CC) $(OBJ) -o $(FILE) $(LDLIBPATH) $(CFLAGS)

$(OBJ)		: $(HDRS)
