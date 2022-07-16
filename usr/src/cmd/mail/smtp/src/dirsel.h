/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)mail:smtp/src/dirsel.h	1.3"
/* ident "@(#)dirsel.h	1.2 'attmail mail(1) command'" */
typedef struct {
	char *name;	/* last component of pathname */
	int namelen;	/* strlen(name) */
	long age;	/* seconds of unmodified existence */
	char isdir;	/* 1 --> it's a directory */
} dirent;

extern int dirsel();
