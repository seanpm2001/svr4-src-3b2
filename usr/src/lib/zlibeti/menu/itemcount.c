/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)sti:menu/itemcount.c	1.2"
#include "private.h"

int
item_count (m)
register MENU *m;
{
  if (m) {
    return Nitems(m);
  }
  return -1;
}
