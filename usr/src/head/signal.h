/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifndef _SIGNAL_H
#define _SIGNAL_H

#ident	"@(#)head:signal.h	1.5.3.2"

typedef int 	sig_atomic_t;

extern char *_sys_siglist[];
extern int _sys_nsig;

#include <sys/signal.h>

#if defined(__STDC__)

extern void (*signal(int, void (*)(int)))(int);
extern int raise(int);

#if __STDC__ == 0 || defined(_POSIX_SOURCE) || defined(_XOPEN_SOURCE)
#include <sys/types.h>
extern int kill(pid_t, int);
extern int sigaction(int, const struct sigaction *, struct sigaction *);
extern int sigaddset(sigset_t *, int);
extern int sigdelset(sigset_t *, int);
extern int sigemptyset(sigset_t *);
extern int sigfillset(sigset_t *);
extern int sigismember(sigset_t *, int);
extern int sigpending(sigset_t *);
extern int sigprocmask(int, sigset_t *, sigset_t *);
extern int sigsuspend(sigset_t *);
#endif

#if __STDC__ == 0
extern int gsignal(int);
extern void (*sigset(int, void (*)(int)))(int);
extern int sighold(int);
extern int sigrelse(int);
extern int sigignore(int);
extern int sigpause(int);
extern int (*ssignal(int, int (*)(int)))(int);
#endif

#else
extern	void(*signal())();
extern  void(*sigset())();

#endif 	/* __STDC__ */

#endif 	/* _SIGNAL_H */
