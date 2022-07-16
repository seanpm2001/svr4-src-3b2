/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)nlp:lib/filters/search.c	1.4"
/* EMACS_MODES: !fill, lnumb, !overwrite, !nodelete, !picture */

#include "string.h"

#include "lp.h"
#include "filters.h"

/**
 ** search_filter() - SEARCH INTERNAL FILTER TABLE FOR FILTER BY NAME
 **/

_FILTER *
#if	defined(__STDC__)
search_filter (
	char *			name
)
#else
search_filter (name)
	register char		*name;
#endif
{
	register _FILTER	*pf;

	for (pf = filters; pf->name; pf++)
		if (STREQU(pf->name, name))
			break;
	return (pf->name? pf : 0);
}
