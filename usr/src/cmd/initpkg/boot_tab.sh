#!/sbin/sh
#	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)initpkg:boot_tab.sh	1.1.1.1"

if u3b2
then echo "/dev/dsk/c1d0s2	  /dev/rdsk/c1d0s2 /usr	    s5	   1	    yes	    -
" > boot_tab
fi
