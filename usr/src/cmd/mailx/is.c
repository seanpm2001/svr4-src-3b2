/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)mailx:is.c	1.2.1.1"
#ident "@(#)is.c	1.3 'attmail mail(1) command'"
#ident	"@(#)mailx:is.c	1.1"
#ident "@(#)is.c	2.6 'attmail mail(1) command'"
#include "rcv.h"
#include <pwd.h>

static int isit();

/*
 * isheader(lp, ctf) - check if lp is header line and return type
 *	lp	-> 	pointer to line
 *	ctfp	->	continuation flag (should be FALSE the first time
 *			isheader() is called on a message.  isheader() sets
 *			it for the remaining calls to that message)
 * returns
 *	FALSE	->	not header line
 *	H_*     ->	type of header line found.
 */
int
isheader(lp, ctfp)
char	*lp;
int	*ctfp;
{
	register char	*p, *q;
	register int	i;

	p = lp;
	while((*p) && (*p != '\n') && (isspace(*p))) {
		p++;
	}
	if((*p == NULL) || (*p == '\n')) {
		/* blank line */
		return (FALSE);
	}

	if ((*ctfp) && ((*lp == ' ') || (*lp == '\t'))) {
		return(H_CONT);
	}

	*ctfp = FALSE;
	for (i = 1; i < H_CONT; i++) {
		if (!isit(lp, i)) {
			continue;
		}
		if ((i == H_FROM) || (i == H_FROM1)) {
			/*
			 * Should NEVER get 'From ' or '>From ' line on stdin
			 * if invoked as mail (rather than rmail) since
			 * 'From ' and/or '>From ' lines are generated by
			 * program itself. Therefore, if it DOES match and
			 * ismail == TRUE, it must be part of the content.
			 */
			if (sending && ismail) {
				return (FALSE);
			}
		}
		*ctfp = TRUE;
		return (i);
	}
	/*
	 * Check if name: value pair
 	 */
	if ((p = strpbrk(lp, ":")) != NULL ) {
		for(q = lp; q < p; q++)  {
			if ( (!isalnum(*q)) && (*q != '-') && (*q != '>'))  {
				return(FALSE);
			}
		}
		*ctfp = TRUE;
		return(H_NAMEVALUE);
	}
	return(FALSE);
}

/*
 * isit(lp, type) -- case independent match of "name" portion of 
 *		"name: value" pair
 *	lp	->	pointer to line to check
 *	type	->	type of header line to match
 * returns
 *	TRUE	-> 	lp matches header type (case independent)
 *	FALSE	->	no match
 */
static int
isit(lp, type)
register char 	*lp;
register int	type;
{
	register char	*p;

	for (p = header[type].tag; *lp && *p; lp++, p++) {
		if (toupper(*p) != toupper(*lp))  {
			return(FALSE);
		}
	}
	if (*p == NULL) {
		return(TRUE);
	}
	return(FALSE);
}

/*
 * istext(line, size) - check for text characters
 */
int
istext(lp, size)
	char	*lp;
	long 	size;
{
	register unsigned char *line = (unsigned char*)lp;
	register unsigned char *ep;
	register c;
	
	for (ep = line+size; --ep >= line; ) {
		c = *ep;
		if ((!isprint(c)) && (!isspace(c)) &&
		    /* Since backspace is not included in either of the */
		    /* above, must do separately                        */
		    (c != '\b')) {
			return(FALSE);
		}
	}
	return(TRUE);
}

/*
 * linecount (line, size) - determine the number of lines in a printable
 *                          file.
 */
int
linecount(lp, size)
	char	*lp;
	int 	size;
{
	register unsigned char	*line = (unsigned char*)lp;
	register unsigned char *ch;
	register c;
	register int count;

	count = 0;
	for (ch = line+size; --ch >= line;)
	{
		c = *ch;
		if (c == '\n')
			count++;
		continue;
	}
	return (count); 
}
