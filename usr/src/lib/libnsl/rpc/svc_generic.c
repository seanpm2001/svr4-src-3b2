/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)librpc:svc_generic.c	1.3"

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*	PROPRIETARY NOTICE (Combined)
*
* This source code is unpublished proprietary information
* constituting, or derived under license from AT&T's UNIX(r) System V.
* In addition, portions of such source code were derived from Berkeley
* 4.3 BSD under license from the Regents of the University of
* California.
*
*
*
*	Copyright Notice 
*
* Notice of copyright on this source code product does not indicate 
*  publication.
*
*	(c) 1986,1987,1988.1989  Sun Microsystems, Inc
*	(c) 1983,1984,1985,1986,1987,1988,1989  AT&T.
*          All rights reserved.
*/ 
#if !defined(lint) && defined(SCCSIDS)
static char sccsid[] = "@(#)svc_generic.c 1.21 89/02/28 Copyr 1988 Sun Micro";
#endif

/*
 * svc_generic.c, Server side for RPC. 
 *
 */

#include <stdio.h>
#include <rpc/rpc.h>
#include <errno.h>
#ifdef SYSLOG
#include <sys/syslog.h>
#else
#define LOG_ERR 3
#endif /* SYSLOG */
#include <rpc/nettype.h>

extern int errno;
extern int t_errno;
extern char *t_errlist[];

extern char *strdup();

/*
 * The highest level interface for server creation. 
 * It tries for all the nettokens in that particular class of token
 * and returns the number of handles it can create and/or find.
 *
 * It creates a link list of all the handles it could create.
 * If svc_create() is called multiple times, it uses the handle
 * created earlier instead of creating a new handle every time.
 */
int
svc_create(dispatch, prognum, versnum, nettype)
	void (*dispatch)();	/* Dispatch function */
	u_long prognum;		/* Program number */
	u_long versnum;		/* Version number */
	char *nettype;		/* Networktype token */
{
	struct xlist {
		SVCXPRT *xprt;		/* Server handle */
		struct xlist *next;	/* Next item */
	} *l;
	static struct xlist *xprtlist;	/* A link list of all the handles */
	int num = 0;
	SVCXPRT *xprt;
	struct netconfig *nconf;
	int net;

	if ((net = _rpc_setconf(nettype)) == 0) {
		(void) syslog(LOG_ERR, "svc_create: unknown protocol");
		return (0);
	}
	while (nconf = _rpc_getconf(net)) {
		for (l = xprtlist; l; l = l->next) {
			if (strcmp(l->xprt->xp_netid, nconf->nc_netid) == 0) {
				/* Found an old one, use it */
				(void) rpcb_unset(prognum, versnum, nconf);
				if (svc_reg(l->xprt, prognum, versnum,
					dispatch, nconf) == FALSE)
					(void) syslog(LOG_ERR,
		"svc_create: could not register prog %d vers %d on %s",
					prognum, versnum, nconf->nc_netid);
				else
					num++;
				break;
			}
		}
		if (l == (struct xlist *)NULL) {
			/* It was not found. Now create a new one */
			xprt = svc_tp_create(dispatch, prognum, versnum, nconf);
			if (xprt) {
				l = (struct xlist *)malloc(sizeof (struct xlist));
				if (l == (struct xlist *)NULL) {
					(void) syslog(LOG_ERR,
						"svc_create: no memory");
					return (0);
				}
				l->xprt = xprt;
				l->next = xprtlist;
				xprtlist = l;
				num++;
			}
		}
	}
	_rpc_endconf();
	/*
	 * In case of num == 0; the error messages are generated by the
	 * underlying layers; and hence not needed here.
	 */
	return (num);
}

/*
 * The high level interface to svc_tli_create().
 * It tries to create a server for "nconf" and registers the service
 * with the rpcbind. It calls svc_tli_create();
 */
SVCXPRT *
svc_tp_create(dispatch, prognum, versnum, nconf)
	void (*dispatch)();	/* Dispatch function */
	u_long prognum;		/* Program number */
	u_long versnum;		/* Version number */
	struct netconfig *nconf;/* Netconfig structure for the network */
{
	SVCXPRT *xprt;

	if (nconf == (struct netconfig *)NULL) {
		(void) syslog(LOG_ERR,
		"svc_tp_create: invalid netconfig structure for prog %d vers %d",
				prognum, versnum);
		return ((SVCXPRT *)NULL);
	}
	xprt = svc_tli_create(RPC_ANYFD, nconf, (struct t_bind *)NULL, 0, 0);
	if (xprt == (SVCXPRT *)NULL)
		return ((SVCXPRT *)NULL);
	(void) rpcb_unset(prognum, versnum, nconf);
	if (svc_reg(xprt, prognum, versnum, dispatch, nconf) == FALSE) {
		(void) syslog(LOG_ERR,
		"svc_tp_create: Could not register prog %d vers %d on %s",
				prognum, versnum, nconf->nc_netid);
		SVC_DESTROY(xprt);
		return ((SVCXPRT *)NULL);
	}
	return (xprt);
}

/*
 * If fd is RPC_ANYFD, then it opens a fd for the given transport
 * provider (nconf cannot be NULL then). If the t_state is T_UNBND and
 * bindaddr is NON-NULL, it performs a t_bind using the bindaddr. For
 * NULL bindadr and Connection oriented transports, the value of qlen
 * is set to 8.
 *
 * If sendsz or recvsz are zero, their default values are chosen.
 */
SVCXPRT *
svc_tli_create(fd, nconf, bindaddr, sendsz, recvsz)
	register int fd;		/* Connection end point */
	struct netconfig *nconf;	/* Netconfig struct for nettoken */
	struct t_bind *bindaddr;	/* Local bind address */
	u_int sendsz;			/* Max sendsize */
	u_int recvsz;			/* Max recvsize */
{
	register SVCXPRT *xprt = NULL;	/* service handle */
	struct t_info tinfo;		/* transport info */
	struct t_bind *tres = NULL;	/* bind info */
	bool_t madefd = FALSE;		/* whether fd opened here  */
	int state;			/* state of the transport provider */

	if (fd == RPC_ANYFD) {
		if (nconf == (struct netconfig *)NULL) {
			(void) syslog(LOG_ERR, "svc_tli_create: invalid netconfig");
			return ((SVCXPRT *)NULL);
		}
		fd = t_open(nconf->nc_device, O_RDWR, &tinfo);
		if (fd == -1) {
			(void) syslog(LOG_ERR,
			"svc_tli_create: could not open connection for %s",
					nconf->nc_netid);
			goto freedata;
		}
		madefd = TRUE;
		state = T_UNBND;
	} else {
	   	/*
		 * It is an open descriptor. Sync it & get the transport info.
		 */
		if ((state = t_sync(fd)) == -1) {
			(void) syslog(LOG_ERR, "svc_tli_create: could not do t_sync: %s",
					t_errlist[t_errno]);
			return ((SVCXPRT *)NULL);
		}
		if (t_getinfo(fd, &tinfo) == -1) {
			(void) syslog(LOG_ERR, "svc_tli_create: could not get transport information");
			return ((SVCXPRT *)NULL);
		}
	}

	/*
	 * If the fd is unbound, try to bind it.
	 * In any case, try to get its bound info in tres
	 */
	tres = (struct t_bind *)t_alloc(fd, T_BIND, T_ADDR);
	if (tres == NULL) {
		(void) syslog(LOG_ERR, "svc_tli_create: No memory!");
		goto freedata;
	}

	switch(state) {
	case T_UNBND:
		if (bindaddr) {
			if (t_bind(fd, bindaddr, tres) == -1) {
				(void) syslog(LOG_ERR,
					"svc_tli_create: could not bind: %s",
						t_errlist[t_errno]);
				goto freedata;
			}
			/*
			 * Should compare the addresses only if addr.len
			 * was non-zero
			 */
			if (bindaddr->addr.len &&
				(memcmp(bindaddr->addr.buf, tres->addr.buf,
					(int)tres->addr.len) != 0)) {
				(void) syslog(LOG_ERR,
			"svc_tli_create: could not bind to requested address: %s",
						t_errlist[t_errno]);
				goto freedata;
			}
		} else {
			tres->qlen = 8; /* Chosen Arbitrarily */
			tres->addr.len = 0;
			if (t_bind(fd, tres, tres) == -1) {
				(void) syslog(LOG_ERR,
					"svc_tli_create: could not bind: %s",
						t_errlist[t_errno]);
				goto freedata;
			}
		}
		break;

	case T_IDLE:
		if (bindaddr) {
			/* Copy the entire stuff in tres */
			if (tres->addr.maxlen < bindaddr->addr.len) {
				(void) syslog(LOG_ERR,
					"svc_tli_create: illegal netbuf length");
				goto freedata;
			}
			tres->addr.len = bindaddr->addr.len;
			(void) memcpy(tres->addr.buf, bindaddr->addr.buf,
					(int)tres->addr.len);
		} else
			/*
			 * XXX: t_getname() call to be used here. TBD
			 */
			tres->addr.len = 0;
		break;
	default:
		(void) syslog(LOG_ERR,
		"svc_tli_create: connection in a wierd state (%d)", state);
		goto freedata;
	}

	/*
	 * call transport specific function.
	 */
	switch(tinfo.servtype) {
		case T_COTS_ORD:
		case T_COTS:
			xprt = svc_vc_create(fd, sendsz, recvsz);
			break;

		case T_CLTS:
			xprt = svc_dg_create(fd, sendsz, recvsz);
			break;
		default:
                	(void) syslog(LOG_ERR, "svc_tli_create: bad service type");
                	goto freedata;
        }
	if (xprt == (SVCXPRT *)NULL)
		/*
		 * The error messages here are spitted out by the lower layers:
		 * svc_vc_create() and svc_dg_create().
		 */
		goto freedata;

	/* fill in the other xprt information */

	/* Assign the local bind address */
	xprt->xp_ltaddr = tres->addr;
	tres->addr.buf = NULL;
	(void) t_free((char *)tres, T_BIND);

	/* Allocate space for the remote bind info */
	xprt->xp_rtaddr.len = 0;
	xprt->xp_rtaddr.maxlen = _rpc_get_a_size(tinfo.addr);
	if ((xprt->xp_rtaddr.buf = mem_alloc(xprt->xp_rtaddr.maxlen)) == NULL) {
		(void) syslog(LOG_ERR, "svc_tli_create: No memory!");
		goto freedata;
	}

	xprt->xp_port = -1;	/* To show that it is tli based. Switch */

	if (nconf) {
		xprt->xp_netid = strdup(nconf->nc_netid);
		xprt->xp_tp = strdup(nconf->nc_device);
	}
	return (xprt);

freedata:
	if (madefd)
		(void) t_close(fd);
	if (tres)
		(void) t_free((char *)tres, T_BIND);
	if (xprt) {
		if (!madefd) /* so that svc_destroy doesnt close fd */
			xprt->xp_fd = RPC_ANYFD;
		SVC_DESTROY(xprt);
	}
	return ((SVCXPRT *)NULL);
}
