/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)curses:screen/_insstr.c	1.1"

#define		NOMACROS
#include	"curses_inc.h"

insstr(s)
char	*s;
{
    return (winsstr(stdscr, s));
}
