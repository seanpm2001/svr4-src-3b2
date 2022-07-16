/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)mail:strmove.c	1.1"
#ident "@(#)strmove.c	1.2 'attmail mail(1) command'"
/*
    NAME
	strmove - copy a string, permitting overlaps

    SYNOPSIS
	void strmove(char *to, char *from)

    DESCRIPTION
	strmove() acts exactly like strcpy() with the additional
	guarantee that it will work with overlapping strings.
	It does it left-to-right, a byte at a time.
*/

void strmove(to, from)
char *to, *from;
{
    while ((*to++ = *from++) != 0)
	;
}
