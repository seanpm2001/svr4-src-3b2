#	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)autoconfig:autoconfig.mk	1.11"

ROOT =
DASHO = -O
INC = $(ROOT)/usr/include
FRC =
INS = install
SYMLINK = :

all:
	@for i in cunix ckmunix nodgmon ckconfig ifile4.0 map4.0 fsrootboot;\
	do\
		cd  $$i;\
			echo "====== $(MAKE) -f $$i.mk \"MAKE=$(MAKE)\" \"AS=$(AS)\" \"CC=$(CC)\" \"LD=$(LD)\" \"FRC=$(FRC)\" \"INC=$(INC)\" \"MORECPP=$(MORECPP)\" \"DASHO=$(DASHO)\" ";\
		$(MAKE) -f $$i.mk "MAKE=$(MAKE)" "AS=$(AS)" "CC=$(CC)" "LD=$(LD)" "FRC=$(FRC)" "INC=$(INC)" "MORECPP=$(MORECPP)" "DASHO=$(DASHO)"; \
		cd .. ;\
	done;\
	wait

install:
	@for i in cunix ckmunix nodgmon ckconfig ifile4.0 map4.0 fsrootboot;\
	do\
		cd  $$i;\
			echo "====== $(MAKE) -f $$i.mk install \"MAKE=$(MAKE)\" \"AS=$(AS)\" \"CC=$(CC)\" \"LD=$(LD)\" \"FRC=$(FRC)\" \"INC=$(INC)\" \"MORECPP=$(MORECPP)\" \"DASHO=$(DASHO)\" \"SYMLINK=$(SYMLINK)\" \"INS=$(INS)\" ";\
		$(MAKE) -f $$i.mk install "MAKE=$(MAKE)" "AS=$(AS)" "CC=$(CC)" "LD=$(LD)" "FRC=$(FRC)" "INC=$(INC)" "MORECPP=$(MORECPP)" "DASHO=$(DASHO)" "SYMLINK=$(SYMLINK)" "INS=$(INS)" ;\
		cd .. ;\
	done;\
	wait

clean:
	@for i in cunix ckmunix nodgmon ckconfig ifile4.0 map4.0 fsrootboot;\
	do\
		cd $$i;\
		$(MAKE) -f $$i.mk clean; \
		cd .. ;\
	done

clobber:
	@for i in cunix ckmunix nodgmon ckconfig ifile4.0 map4.0 fsrootboot;\
	do\
		cd $$i;\
		$(MAKE) -f $$i.mk clobber ;\
		cd .. ;\
	done

FRC:
