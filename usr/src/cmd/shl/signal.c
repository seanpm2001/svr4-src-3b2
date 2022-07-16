/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)shl:signal.c	1.13.2.1"
#include	"defs.h"
#include	<unistd.h>
void 	(*(sig[NSIG]))() = 
{
	0,
	hangup,
	SIG_IGN,
	SIG_IGN,
	SIG_IGN,
	SIG_IGN,
	SIG_IGN,
	SIG_IGN,
	SIG_IGN,
	SIG_IGN,
	SIG_IGN,
	SIG_IGN,
	SIG_IGN,
	SIG_IGN,
	SIG_IGN,
	SIG_IGN,
	SIG_IGN,
	SIG_IGN,
	child_death,
	SIG_IGN
};

void	(*(save_sig[NSIG]))();
	
void hangup()
{

	int i;

	/* Try to ignore hangup before login shell sends another hangup */

	signal(SIGHUP, SIG_IGN);

	signal(SIGCLD, SIG_IGN);

	for (i = 1; i <= max_index; ++i)
		if (layers[i])
			kill(-layers[i]->p_grp, SIGHUP);

	reset();

	exit(5);
}


void child_death(n)
	int n;
{
	pid_t pid;
	int status;
	
	pid = wait(&status);

	clean_up(pid);
}	


clean_up(pid)
	pid_t pid;
{
	int i;

	for (i = 1; i <= max_index; ++i)
		if (layers[i])
			if (pid == layers[i]->p_grp)
				break;


	if (i > max_index)
	{
		signal(SIGCLD, child_death);
		return;
	}

	free_layer(i);

	if (i == top())
	{
		if (ioctl(cntl_chan_fd, SXTIOCSWTCH, 0) != SYSERROR)
			pop();
		else
		{
			fprintf(stderr, "switch to channel 0 failed (errno = %d)\n", errno);
			reset();
			exit(4);
		}
	}

	signal(SIGCLD, child_death);
}


setsig()
{
	int i;

	for (i = 1; i < NSIG; ++i)
		save_sig[i] = signal(i, sig[i]);

	save_sig[SIGHUP] = SIG_DFL;
}

restore_sig()
{
	int i;
	
	for (i = 1; i < NSIG; ++i)
		signal(i, save_sig[i]);
}

void sigalrm()
{
	reset();
	/* must print error message this way because printing before reset 
	   causes message to be truncated on vax. */
	fcntl(real_tty_fd,F_DUPFD,2); 
	fprintf(stderr,"warning: there may still be shl layers running\n");
	exit(12);
}
