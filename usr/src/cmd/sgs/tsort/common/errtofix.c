/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)tsort:errtofix.c	1.1"

/*	Set tofix string.
*/

#include	"errmsg.h"


void
errtofix( str )
char	*str;
{
	Err.tofix = str;
}
