/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libl:lib/yywrap.c	1.7"
#if defined(__cplusplus) || defined(__STDC__)
int yywrap(void)
#else
yywrap()
#endif
{
	return(1);
}
