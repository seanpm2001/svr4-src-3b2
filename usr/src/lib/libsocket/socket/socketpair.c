/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)libsocket:socketpair.c	1.10"

/*
 * +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * 		PROPRIETARY NOTICE (Combined)
 * 
 * This source code is unpublished proprietary information
 * constituting, or derived under license from AT&T's UNIX(r) System V.
 * In addition, portions of such source code were derived from Berkeley
 * 4.3 BSD under license from the Regents of the University of
 * California.
 * 
 * 
 * 
 * 		Copyright Notice 
 * 
 * Notice of copyright on this source code product does not indicate 
 * publication.
 * 
 * 	(c) 1986,1987,1988.1989  Sun Microsystems, Inc
 * 	(c) 1983,1984,1985,1986,1987,1988,1989  AT&T.
 * 	          All rights reserved.
 *  
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/errno.h>
#include <sys/stream.h>
#include <sys/ioctl.h>
#include <sys/stropts.h>
#include <sys/tihdr.h>
#include <sys/sockmod.h>
#include <sys/socket.h>
#include <sys/tiuser.h>
#include <fcntl.h>

extern int	errno;

int
socketpair(family, type, protocol, sv)
	register int			family;
	register int			type;
	register int			protocol;
	register int			sv[2];

{
	register struct _si_user	*siptr;
	register struct _si_user	*nsiptr;
	register struct netconfig	*net;
	struct bind_ux			bind_ux;
	struct bind_ux			nbind_ux;
	struct t_call			sndcall;
	int				size;
	void				*nethandle;

	if (family != AF_UNIX) {
		errno = EPROTONOSUPPORT;
		return -1;
	}

	/* create endpoints ns and s
	 */
        if ((net = _s_match(family, type, protocol, &nethandle)) == NULL)
                return -1;
 
	if (strcmp(net->nc_proto, NC_NOPROTO) != 0)
		protocol = 0;
        nsiptr = _s_open(net->nc_device, protocol);
	siptr = _s_open(net->nc_device, protocol);

	endnetconfig(nethandle);

	if (nsiptr == NULL || siptr == NULL)
		return -1;

	(void)memset((caddr_t)&nbind_ux, 0, sizeof(nbind_ux));
	(void)memset((caddr_t)&bind_ux, 0, sizeof(bind_ux));
	(void)memset((caddr_t)&sndcall, 0, sizeof(sndcall));

	/* Bind each end.
	 */
	size = sizeof(nbind_ux);
	if (_bind(nsiptr, NULL, 0, (caddr_t)&nbind_ux, &size) < 0)
		goto bad;
	if (size != sizeof(nbind_ux)) {
		errno = EPROTO;
		goto bad;
	}

	if (_bind(siptr, NULL, 0, (caddr_t)&bind_ux, &size) < 0)
		goto bad;
	if (size != sizeof(nbind_ux)) {
		errno = EPROTO;
		goto bad;
	}

	if (type == SOCK_DGRAM) {
		/* connect s->ns
		 */
		sndcall.addr.buf = (caddr_t)&nbind_ux;
		sndcall.addr.len = sizeof(nbind_ux);
		if (_connect2(siptr, &sndcall) < 0)
			goto bad;

		/* connect ns->s
		 */
		sndcall.addr.buf = (caddr_t)&bind_ux;
		sndcall.addr.len = sizeof(bind_ux);
		if (_connect2(nsiptr, &sndcall) < 0)
			goto bad;

		/* return descripters for each end
		 */
		sv[0] = nsiptr->fd;
		sv[1] = siptr->fd;

		return sv[1];
	}
	else	{
		register int retval, s2, cntlflag;
		struct   timeval timeout;
		fd_set	 writefds;

		/* set the queue length on s.
		 */
		if (listen(siptr->fd, 1) < 0)
			goto bad;
	
		/* set ns no delay mode.
		 */
		cntlflag = fcntl(nsiptr->fd, F_GETFL, 0);
	        (void)fcntl(nsiptr->fd, F_SETFL, cntlflag | O_NDELAY);
	
		/* send the connect ns->s.
		 */
		sndcall.addr.buf = (caddr_t)&bind_ux;
		sndcall.addr.len = sizeof(bind_ux);
		if (_connect2(nsiptr, &sndcall) < 0 && errno != EINPROGRESS) {
			goto bad;
		}
	
		/* pick up the connect indication on s2
		 */
		if ((s2 = _accept(siptr, NULL, NULL)) < 0) {
			goto bad;
		}

		/* wait until the confirmation arrives
		 */
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;
		FD_ZERO(&writefds);
		FD_SET(nsiptr->fd, &writefds);
		if ((retval = select(32, NULL, &writefds, NULL, &timeout)) < 0 
					|| retval != 1)  {
			(void)close(s2);
			goto bad;
		}

		/* Reset the O_NDELAY flag.
		 */
	        (void)fcntl(nsiptr->fd, F_SETFL, cntlflag);

		sv[0] = nsiptr->fd;
		sv[1] = s2;

		_s_close(siptr);

		return sv[1];
	}
bad:
	_s_close(nsiptr);
	_s_close(siptr);

	return -1;
}
