/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)oamintf:intf_include/menutrace.h	1.1"

/* define menu search struct */

typedef struct taskdescr {
	int	ident;		/* node counter */
	int	mark;		/* found marker */
	char	*tname;		/* task name */
	char	*action;	/* action field (opt) */
	struct taskdescr *next;	/* sibling in hierarchy */
	struct taskdescr *child; /* child in hierarchy */
	struct taskdescr *parent; /* parent node pointer */
	struct taskdescr *thread; /* thread through hierarchy */
} TASKDESC;
