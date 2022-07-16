#!/sbin/sh
#	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)filemgmt:bin/devices.sh	1.1"
FSYS=$1
while read bdev cdev mountp dummy
do
	case $bdev in
	'#'*| ' ')
		continue;;
	esac
	if [ "$mountp" =  "$FSYS" ]
	then
		if [ -b $bdev -o -c $bdev ]
		then
			echo $bdev
		fi
	fi
done < /etc/vfstab
