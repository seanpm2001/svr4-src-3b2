/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

/*
 * Copyright  (c) 1986 AT&T
 *	All Rights Reserved
 */
#ident	"@(#)vm.vt:src/vt/redraw.c	1.1"

#include	<curses.h>
#include	"wish.h"
#include	"vt.h"
#include	"vtdefs.h"

vt_redraw()
{
	wrefresh(curscr);
}
