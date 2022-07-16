/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)bkrs:bklib.d/bkstrcmp.c	1.3"

extern int strcmp();

/* strcmp + handle NULL pointers */
int
bkstrcmp( a, b )
char *a, *b;
{
	if( a && b ) return( strcmp( a, b ) );
	if( a ) return( -1 );
	return( b != (char *)0 );
}
