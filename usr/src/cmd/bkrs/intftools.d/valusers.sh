#!/sbin/sh
#	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T
#	  All Rights Reserved

#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any
#	actual or intended publication of such source code.

#ident	"@(#)bkrs:intftools.d/valusers.sh	1.2"
# validate that list of users is in password file
USERS="$1"
USERS=`echo $USERS | sed -e "s/,/ /g"`
for i in $USERS
do
	grep "^${i}:" /etc/passwd >/dev/null
	if [ $? -ne 0 ]
	then
		exit 1
	fi
done
exit 0
