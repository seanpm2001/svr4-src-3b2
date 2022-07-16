/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)shl:system.c	1.4.2.1"
#include	"defs.h"
#include	<unistd.h>

system(s)
const	char	*s;
{
	int	status;
	pid_t 	pid;
	pid_t 	i;

	signal(SIGCLD, SIG_DFL);

	if((pid = fork()) == 0) 
	{
		(void) execl("/bin/ps", "ps", "-f", "-g", s, 0);
		_exit(127);
	}
	
	for (;;)
	{
		i = wait(&status);

		if (i != (pid_t)-1 && i != pid)
			clean_up(i);
		else
		{
			signal(SIGCLD, child_death);
			return;
		}
	}
}
		

