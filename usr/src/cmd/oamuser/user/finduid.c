/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)oamuser:user/finduid.c	1.2"

#include	<sys/types.h>
#include	<stdio.h>
#include	<userdefs.h>

extern void exit();
extern uid_t findnextuid();

/* return the next available uid */
main()
{
	uid_t uid = findnextuid();
	if( uid == -1 )
		exit( EX_FAILURE );
	(void) fprintf( stdout, "%ld\n", uid );
	exit( EX_SUCCESS );
	/*NOTREACHED*/
}
