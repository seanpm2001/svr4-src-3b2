#!/sbin/sh
#	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)initpkg:rstab.sh	1.1.2.1"
if u3b2
then echo "
#	place share(1M) commands here for automatic execution
#	on entering init state 3.
#
#	share [-F fstype] [ -o options] [-d \"<text>\"] <pathname> <resource>
#	.e.g,
#	share -F rfs -d \"/var/news\"  /var/news NEWS
" >dfstab
fi
