/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)head:nsaddr.h	1.9"

#ifndef _NSADDR_H
#define _NSADDR_H

/* 	
	stoa - convert string to address
	atos - convert address to string
	header file
*/

#define	OCT	0	/* octal type */
#define	HEX	1	/* hexadecimal type */
#define	RAW	2	/* string type */
#define KEEP	8	/* keep protocol field	*/

#ifndef _SYS_TIUSER_H
#include <sys/tiuser.h>
#endif

struct address {
	char		*protocol;
	struct netbuf	addbuf;
};

struct netbuf	*stoa(char *, struct netbuf *);
char 		*atos(char *, struct netbuf *, int);
struct address	*astoa(char *, struct address *);
char		*aatos(char *, struct address *, int);

#endif 	/* _NSADDR_H */
