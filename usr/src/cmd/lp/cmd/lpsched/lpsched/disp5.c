/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)lp:cmd/lpsched/disp5.c	1.7"

#include "dispatch.h"

extern int		Net_fd;

extern MESG *		Net_md;

/**
 ** s_child_done()
 **/

void
#if	defined(__STDC__)
s_child_done (
	char *			m,
	MESG *			md
)
#else
s_child_done (m, md)
	char			*m;
	MESG			*md;
#endif
{
	long			key;

	short			slot;
	short			status;
	short			err;


	getmessage (m, S_CHILD_DONE, &key, &slot, &status, &err);

	if (
		0 <= slot
	     && slot < ET_Size
	     && Exec_Table[slot].key == key
	     && Exec_Table[slot].md == md
	) {

#if	defined(DEBUG)
	    if (debug) {
		FILE			*fp	= open_logfile("exec");

		time_t			now = time((time_t *)0);

		char			buffer[BUFSIZ];

		extern char		*ctime();

		EXEC *			ep = &Exec_Table[slot];


		if (fp) {
			setbuf (fp, buffer);
			fprintf (
				fp,
		"OKAY: %24.24s slot %d, pid %d, status %d, err %d%s%s\n",
				ctime(&now),
				slot,
				ep->pid,
				status,
				err,
				(ep->type == EX_INTERF? ", req " : ""),
				(ep->type == EX_INTERF?
				    ep->ex.request->secure->req_id : "")
			);
			close_logfile (fp);
		}
	    }
#endif
		/*
		 * Remove the message descriptor from the listen
		 * table, then forget about it; we don't want to
		 * accidently match this exec-slot to a future,
		 * unrelated child.
		 */
		DROP_MD (Exec_Table[slot].md);
		Exec_Table[slot].md = 0;

		Exec_Table[slot].pid = -99;
		Exec_Table[slot].status = status;
		Exec_Table[slot].errno = err;
		DoneChildren++;

	}

#if	defined(DEBUG)
	else if (debug) {
		FILE			*fp	= open_logfile("exec");

		time_t			now = time((time_t *)0);

		char			buffer[BUFSIZ];

		extern char		*ctime();

		EXEC *			ep = &Exec_Table[slot];


		if (fp) {
			setbuf (fp, buffer);
			fprintf (
				fp,
		"FAKE! %24.24s slot %d, pid unknown, status %d, err %d\n",
				ctime(&now),
				slot,
				status,
				err
			);
			close_logfile (fp);
		}
	}
#endif

	return;
}

/**
 ** r_new_child()
 **/

void
#if	defined(__STDC__)
r_new_child (
	char *			m,
	MESG *			md
)
#else
r_new_child (m, md)
	char			*m;
	MESG			*md;
#endif
{
	char *			name;
	char *			originator_name;

	short			status;

	int			were_waiting	= 0;

	struct strrecvfd	recvfd;

	SSTATUS *		pss;

	PSTATUS *		pps;

	MESG *			new_md;
	MESG *			drop_md;
	MESG *			hold_md;


	getmessage (m, R_NEW_CHILD, &name, &originator_name, &status);
	schedlog (
		"Received R_NEW_CHILD for system %s requested by %s\n",
		name,
		originator_name
	);

	if (!(pss = search_stable(name))) {
		schedlog ("%s is an unknown system\n", name);
		recvfd.fd = -1;	/* So as not to clobber someone else */
		ioctl (Net_fd, I_RECVFD, &recvfd);
		close (recvfd.fd);
		return;
	}


	switch (status) {

	case MOK:
		break;

	case MUNKNOWN:
		/*
		 * The network manager doesn't know about this system.
		 * While strictly speaking this ought not occur, it can
		 * because we can't prevent someone from mucking with
		 * the system table. So if this happens we disable the
		 * printer(s) that go to this system.
		 */
		for (pps = walk_ptable(1); pps; pps = walk_ptable(0))
			if (pps->system == pss)
				disable (pps, CUZ_NOREMOTE, 0);
		return;

	case MNOSTART:
		/*
		 * We get this only in response to our request for a
		 * connection. However, between the time we had asked
		 * and the receipt of this response we may have received
		 * another R_NEW_CHILD originated remotely. So, we try
		 * again later only if we still need to.
		 */
		if (!pss->exec->md) {
			RSTATUS *		prs = pss->exec->ex.request;

			schedlog (
				"Failed contact with %s, retry in %d min.\n",
				name,
				WHEN_NOSTART / MINUTE
			);
			resend_remote (pss, WHEN_NOSTART);
		}
		return;

	default:
		schedlog (
			"Strange status (%d) in R_NEW_CHILD for %s.\n",
			status,
			name
		);
		return;
	}
				    

	if (ioctl(Net_fd, I_RECVFD, &recvfd) == -1) {
		switch (errno) {
		case EBADMSG:
			schedlog ("No file descriptor passed.\n");
			break;

		case ENXIO:
			schedlog ("System server terminated early.\n");
			break;

		case EMFILE:
			schedlog ("Too many open files!\n");
			break;
		}
		return;
	}

	new_md = mconnect(NULL, recvfd.fd, recvfd.fd);
	if (!new_md)
		mallocfail ();

	/*
	 * Save this flag, because in the hustle and bustle below
	 * we may lose the original information.
	 */
	were_waiting = (pss->exec->flags & EXF_WAITCHILD);


	/*
	 * Check for a collision with another system trying to contact us:
	 *
	 *	* We had asked for a connection to this system (i.e. had
	 *	  sent a S_NEW_CHILD message), but this isn't the response
	 *	  to that message.
	 *
	 *	* We already have a connection.
	 *
	 * These cases are handled separately below, but the same
	 * arbitration is used: The system with the name that comes
	 * ``first'' in collating order gets to keep the connection
	 * it originated.
	 */
	if (were_waiting) {

		if (STREQU(Local_System, originator_name)) {
			/*
			 * This is the usual case.
			 */
			schedlog ("Making new connection to %s\n", name);
			hold_md = new_md;
			drop_md = 0;

		} else {
			/*
			 * We have a pending collision, since we
			 * are still waiting for a response to our
			 * connection request (this isn't it). Resolve
			 * the collision now, by either accepting
			 * this response (we'll have to refuse our
			 * real response later) or by refusing this
			 * response.
			 */
			schedlog (
				"Potential collision between %s and %s\n",
				Local_System,
				name
			);
			if (strcmp(Local_System, name) < 0) {
				schedlog ("Take no connection.\n");
				hold_md = 0;
				drop_md = new_md;
			} else {
				schedlog ("Drop this connection.\n");
				hold_md = new_md;
				drop_md = 0;
			}
		}

	} else 
	if (pss->exec->md) {
		MESG *			my_md;
		MESG *			his_md;

		schedlog (
			"Collision between %s and %s!\n",
			Local_System,
			name
		);

		/*
		 * The message descriptor we got last time
		 * MAY NOT be for the connection we originated.
		 * We have to check the "originator_name" to be sure.
		 */
		if (STREQU(Local_System, originator_name)) {
			my_md = new_md;
			his_md = pss->exec->md;
		} else {
			my_md = pss->exec->md;
			his_md = new_md;
		}

		/*
		 * (First means < 0, right?)
		 */
		if (strcmp(Local_System, name) < 0) {
			schedlog ("I win!\n");
			drop_md = his_md;
			hold_md = my_md;
		} else {
			schedlog ("He wins.\n");
			drop_md = my_md;
			hold_md = his_md;
		}

	} else
	{
		schedlog ("Accepting unsolicited connection.\n");
		hold_md = new_md;
		drop_md = 0;
	}

	if (drop_md) {
		if (drop_md == pss->exec->md) {
			schedlog (
				"Dropping fd %d from listen table\n",
				drop_md->readfd
			);
			DROP_MD (drop_md);

			/*
			 * We are probably waiting on a response
			 * to an S_SEND_JOB from the network child
			 * on the other end of the connection we
			 * just dropped. If so, we have to resend the
			 * job through the new channel...yes, we know
			 * we have a new channel, as the only way to
			 * get here is if we're dropping the exising
			 * channel, and we do that only if we have to
			 * pick between it and the new channel.
			 */
			if (pss->exec->flags & EXF_WAITJOB) {
				resend_remote (pss, -1);
				were_waiting = 1;
			}
		} else {
			schedlog (
				"Sending S_CHILD_SYNC M2LATE on %x\n",
				drop_md
			);
			drop_md->type = MD_CHILD;
			mputm (drop_md, S_CHILD_SYNC, M2LATE);
			mdisconnect (drop_md);
		}
	}
	if (hold_md) {
		if (hold_md != pss->exec->md)
		{
			schedlog (
				"Sending S_CHILD_SYNC MOK on %x\n",
				hold_md
			);
			hold_md->type = MD_CHILD;
			mputm (hold_md, S_CHILD_SYNC, MOK);
			pss->exec->md = hold_md;
			if (mlistenadd(pss->exec->md, POLLIN) == -1)
				mallocfail ();
		}
		pss->exec->flags &= ~EXF_WAITCHILD;
	}

	/*
	 * If we still have a connection to the remote system,
	 * and we had been waiting for the connection, (re)send
	 * the job.
	 */
	if (pss->exec->md && were_waiting)
		schedule (EV_SYSTEM, pss);

	return;
}

/**
 ** r_send_job()
 **/

void
#if	defined(__STDC__)
r_send_job (
	char *			m,
	MESG *			md
)
#else
r_send_job (m, md)
	char			*m;
	MESG			*md;
#endif
{
	char *			name;
	char *			sent_msg;
	char *			req_id;

	short			status;
	short			sent_size;

	long			lstatus;
	long			chkbits;

	SSTATUS *		pss;

	RSTATUS *		prs;

	PSTATUS *		pps;

	MESG *			user_md;

	int			x;
	char			buf[MSGMAX];
	char			*s1, *s2, *s3, *s4, *s5, *s6;
	short			h1, h2;
	long			l1, l2, l3;


	getmessage (m, R_SEND_JOB, &name, &status, &sent_size, &sent_msg);
	schedlog ("Received R_SEND_JOB from system %s.\n", name);

	if (!(pss = search_stable(name))) {
		schedlog ("%s is an unknown system\n", name);
		return;
	}

	prs = pss->exec->ex.request;
	if (!(prs->request->outcome & RS_SENDING)) {
		schedlog ("Unexpected R_SEND_JOB--no request sent!\n");
		return;
	}
	if (!(pss->exec->flags & EXF_WAITJOB)) {
		schedlog ("Unexpected R_SEND_JOB--not waiting!\n");
		return;
	}

	switch (status) {

	case MOK:
		break;

	case MTRANSMITERR:
		schedlog ("Received MTRANSMITERR from %s, retrying.\n", name);
		resend_remote (pss, 0);
		return;

	default:
		schedlog ("Odd status in R_SEND_JOB, %d!\n", status);
		return;

	}

	switch (mtype(sent_msg)) {

	case R_PRINT_REQUEST:
		/*
		 * Clear the waiting-for-R_SEND_JOB flag now that
		 * we know this message isn't bogus.
		 */
		pss->exec->flags &= ~EXF_WAITJOB;

		getmessage (
			sent_msg,
			R_PRINT_REQUEST,
			&status,
			&req_id,
			&chkprinter_result
		);

		prs->request->outcome &= ~RS_SENDING;
		prs->printer->status &= ~PS_BUSY;

		if (status == MOK) {
			schedlog("S_SEND_JOB had succeeded\n");
			prs->request->outcome |= RS_SENT;
			/*
			 * Record the fact that we've sent this job,
			 * to avoid sending it again if we restart.
			 */
			putrequest (prs->req_file, prs->request);

		} else {
			schedlog ("S_SEND_JOB had failed, status was %d!\n", status);
			/*
			 * This is very much like what happens if the
			 * print service configuration changes and causes
			 * a local job to be no longer printable.
			 */
			prs->reason = status;
			cancel (prs, 1);
		}

		break;

	case R_GET_STATUS:

		/* Were we expecting this? */
		if (!(prs->status & RSS_RECVSTATUS))
		{
		    schedlog("Unexpected status report from system:  %s\n",
			     pss->system->name);
		    break;
		}

		/* is the protocol correct? */
		if (pss->system->protocol == S5_PROTO)
		{
		    schedlog("Protocol mismatch:  system %s: got BSD, expected S5\n", pss->system->name);
		    break;
		}

		/* get the message */
		    (void)getmessage(s2, R_GET_STATUS, &status, &s1);

		/* find the printer referred to by s1 */
/*		for (x = 0; (pps = PStatus[x]) != NULL; x++)	*/
		for (pps = walk_ptable(1); pps; pps = walk_ptable(0))
		{
		    if (pps->system != pss)
			continue;
		    if (STREQU(pps->remote_name, s1))
			break;
		}

		/* find the printer ? */
		if (pps == NULL)
		    schedlog("Received status from unknown printer %s\n",
			      s1);

		if (pps)
		{
		    load_bsd_stat(pss, pps);

		    /*
		     * Processed the last printer for pss,
		     * so clear the waiting-for-R_SEND_JOB flag.
		     */
		    pss->exec->flags &= ~EXF_WAITJOB;

		    if (pss->tmp_pmlist)
		    {
			if (pss->pmlist)
			    freelist(pss->pmlist);
			pss->pmlist = pss->tmp_pmlist;
			pss->tmp_pmlist = NULL;
		    }

		    md_wakeup(pss);
		}
		break;

	case R_INQUIRE_PRINTER_STATUS:
		/* expecting this ? */
		if (!(prs->status & RSS_RECVSTATUS))
		{
		    schedlog("Unexpected status report from system %s\n",
			     pss->system->name);
		    return;
		}

		/* protocol ok ? */
		if (pss->system->protocol == BSD_PROTO)
		{
		    schedlog("Protocol mismatch:  system %s: got S5, expected BSD\n", pss->system->name);
		    break;
		}

		(void) getmessage(s2, R_INQUIRE_PRINTER_STATUS, &status,
				  &s1, &s2, &s3, &s4, &s5, &h1, &s6, &l1,
				  &l2);

		/* find the printer referred to by s1 */
/*		for (x = 0; (pps = PStatus[x]) != NULL; x++)	*/
		for (pps = walk_ptable(1); pps; pps = walk_ptable(0))
		{
		    if (pps->system != pss)
			continue;
		    if (STREQU(pps->remote_name, s1))
			break;
		}

		/* find the printer ? */
		if (pps == NULL)
		{
		    schedlog("Received status from unknown printer %s\n",
			      s1);
		    return;
		}

		if (status == MOKMORE || status == MOK)
		    (void) putmessage(buf, R_INQUIRE_REMOTE_PRINTER,
				      MOKMORE, pps->printer->name, s2, s3,
				      s4, s5, h1, s6, l1, l2);
		else
		    (void) putmessage(buf, R_INQUIRE_REMOTE_PRINTER,
				      status, pps->printer->name, s2, s3,
				      s4, s5, h1, s6, l1, l2);
		mesgadd(pss, buf);
		return;


	case R_INQUIRE_REQUEST_RANK:
		/* expecting this ? */
		if (!(prs->status & RSS_RECVSTATUS))
		{
		    schedlog("Unexpected status report from system %s\n",
			     pss->system->name);
		    return;
		}

		/* protocol ok ? */
		if (pss->system->protocol == BSD_PROTO)
		{
		    schedlog("Protocol mismatch:  system %s: got S5, expected BSD\n", pss->system->name);
		    break;
		}

		/*
		 *	if a list of new messages has been created
		 *	free the old list (if any) and point to
		 *	the new one.
		 */
		if (pss->tmp_pmlist)
		{
		    if (pss->pmlist)
			freelist(pss->pmlist);
		    pss->pmlist = pss->tmp_pmlist;
		    pss->tmp_pmlist = NULL;
		}

		(void) getmessage(s2, R_INQUIRE_REQUEST_RANK, &status,
				  &s1, &s2, &l1, &l2, &h1, &s3, &s4, &s5, &l3);

		if (status == MOKMORE || status == MOK)
		    update_req(s1, l3);

		if (status != MOKMORE)
		{
		    /*
		     * Last of status received from the system, so
		     * clear the waiting-for-R_SEND_JOB flag.
		     */
		    pss->exec->flags &= ~EXF_WAITJOB;
		    md_wakeup(pss);
		}
		break;

	case R_CANCEL:
		if (!(prs->request->outcome & RS_CANCELLED)) {
			schedlog ("Unexpected R_CANCEL (not canceled)!\n");
			return;
		}

		getmessage (sent_msg, R_CANCEL, &status, &lstatus, &req_id);
		if (!STREQU(prs->secure->req_id, req_id)) {
			schedlog ("Out of sync on R_CANCEL: wanted %s, got %s\n", prs->secure->req_id, req_id);
			return;
		}

		/*
		 * Clear the waiting-for-R_SEND_JOB flag now that
		 * we know this message isn't bogus.
		 */
		pss->exec->flags &= ~EXF_WAITJOB;

#if	defined(SEND_CANCEL_RESPONSE)
		/*
		 * If there is no message descriptor attached to this
		 * request, the user has hung up, so don't send a message
		 * back.
		 */
		if ((user_md = prs->md)) {
			if (user_md->wait == 0)
			    mputm(user_md, R_CANCEL_REQUEST, status);
			else
			    if (--user_md->wait > 0)
				mputm(user_md, R_CANCEL, MOKMORE, lstatus, req_id);
			    else
				mputm(user_md, R_CANCEL, MOK, lstatus, req_id);
		}
#endif

		prs->request->outcome &= ~RS_SENDING;
		dowait_remote (EX_NOTIFY, prs, 0, (char *)0);

		break;

	case R_JOB_COMPLETED:
		if (!(prs->request->outcome & RS_DONE)) {
			schedlog ("Unexpected R_JOB_COMPLETED (not done)!\n");
			return;
		}

		schedlog ("Received R_JOB_COMPLETED, request %s\n", prs->secure->req_id);

		/*
		 * Clear the waiting-for-R_SEND_JOB flag now that
		 * we know this message isn't bogus.
		 */
		pss->exec->flags &= ~EXF_WAITJOB;

		getmessage (sent_msg, R_JOB_COMPLETED, &status);
		if (status == MUNKNOWN)
			note (
		"%s refused job completion notice for request %s.\n",
				name,
				prs->secure->req_id
			);

		prs->request->outcome &= ~RS_SENDING;
		dowait_remote (EX_NOTIFY, prs, 0, (char *)0);

		break;

	}

	/*
	 * There may be another request waiting to go out over
	 * the network.
	 */
	schedule (EV_SYSTEM, pss);

	return;
}

/**
 ** s_job_completed()
 **/

void
#if	defined(__STDC__)
s_job_completed (
	char *			m,
	MESG *			md
)
#else
s_job_completed (m, md)
	char			*m;
	MESG			*md;
#endif
{
	char *			req_id;
	char *			errfile;

	short			outcome;

	RSTATUS *		prs;


	getmessage (m, S_JOB_COMPLETED, &outcome, &req_id, &errfile);

	if (!(prs = request_by_id(req_id))) {
		schedlog ("Got S_JOB_COMPLETED for unknown request %s\n", req_id);
		mputm (md, R_JOB_COMPLETED, MUNKNOWN);
		return;
	}

	mputm (md, R_JOB_COMPLETED, MOK);
	dowait_remote (EX_INTERF, prs, outcome, errfile);

	return;
}

void
#if	defined(__STDC__)
load_system (
	char *			m,
	MESG *			md
)
#else
load_system (m, md)
	char *			m;
	MESG *			md;
#endif
{
}

void
#if	defined(__STDC__)
unload_system (
	char *			m,
	MESG *			md
)
#else
unload_system (m, md)
	char *			m;
	MESG *			md;
#endif
{
}
