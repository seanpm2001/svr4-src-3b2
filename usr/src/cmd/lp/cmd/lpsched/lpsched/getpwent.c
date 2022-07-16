/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)nlp:cmd/lpsched/getpwent.c	1.1.1.1"

/*
 * These routines duplicate some of those of "getpwent(3C)". We have
 * them so that we can use our special "open_lpfile()" routine,
 * which opens files and REUSES preallocated buffers. Without
 * this, every new print job will hit malloc with a request for
 * a large buffer; this typically (with most versions of malloc)
 * leads to increased fragmentation of the free memory arena.
 */

#include "sys/types.h"
#include "stdlib.h"
#include "pwd.h"

#include "lp.h"

static char		PASSWD[] = "/etc/passwd";

static FILE		*pwf = NULL;

void
#if	defined(__STDC__)
lp_setpwent (
	void
)
#else
lp_setpwent ()
#endif
{
	if (!pwf)
		pwf = open_lpfile(PASSWD, "r", 0);
	else
		rewind (pwf);
}

void
#if	defined(__STDC__)
lp_endpwent (
	void
)
#else
lp_endpwent ()
#endif
{
	if (pwf) {
		(void) close_lpfile(pwf);
		pwf = 0;
	}
}

struct passwd *
#if	defined(__STDC__)
lp_getpwuid (
	register uid_t		uid
)
#else
lp_getpwuid (uid)
	register uid_t		uid;
#endif
{
	register struct passwd *p;

	lp_setpwent ();

	/*
	 * Call the REAL routine to access the data (not "getpwent()",
	 * though, which will call the REAL "setpwent()" which we don't
	 * want to happen!)
	 */
	while ((p = fgetpwent(pwf)) && p->pw_uid != uid)
		;

	lp_endpwent ();
	return (p);
}

struct passwd *
#if	defined(__STDC__)
lp_getpwnam (
	char *			name
)
#else
lp_getpwnam (name)
	char *			name;
#endif
{
	register struct passwd *p;

	lp_setpwent ();

	/*
	 * Call the REAL routine to access the data (not "getpwent()",
	 * though, which will call the REAL "setpwent()" which we don't
	 * want to happen!)
	 */
	while ((p = fgetpwent(pwf)) && strcmp(name, p->pw_name))
		;

	lp_endpwent ();
	return (p);
}
