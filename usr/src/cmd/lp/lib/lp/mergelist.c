/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)nlp:lib/lp/mergelist.c	1.5"
/* EMACS_MODES: !fill, lnumb, !overwrite, !nodelete, !picture */

/**
 ** mergelist() - ADD CONTENT OF ONE LIST TO ANOTHER
 **/

int
#if	defined(__STDC__)
mergelist (
	char ***		dstlist,
	char **			srclist
)
#else
mergelist (dstlist, srclist)
	register char		***dstlist,
				**srclist;
#endif
{
	if (!srclist || !*srclist)
		return (0);

	while (*srclist)
		if (addlist(dstlist, *srclist++) == -1)
			return (-1);
	return (0);
}
