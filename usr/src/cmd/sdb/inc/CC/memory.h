/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)sdb:inc/CC/memory.h	1.1"
/*ident	"@(#)cfront:incl/memory.h	1.5"*/

#ifndef MEMORYH
#define MEMORYH

extern void
	*memccpy(void*, const void*, int, int),
	*memchr(const void*, int, int),
	*memcpy(void*, const void*, int),
	*memset(void*, int, int);
extern int memcmp(const void*, const void*, int);

#endif
