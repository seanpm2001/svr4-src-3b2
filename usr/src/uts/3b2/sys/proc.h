/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ifndef _SYS_PROC_H
#define _SYS_PROC_H

#ident	"@(#)head.sys:sys/proc.h	11.65"

#include "sys/immu.h"

/*
 * One structure allocated per active process.  It contains all
 * data needed about the process while the process may be swapped
 * out.  Other per-process data (user.h) may be swapped with the
 * process.
 */
typedef struct	proc {
	char	p_stat;			/* status of process */
	char	p_cpu;			/* cpu usage for scheduling */
	u_char	p_curflt;		/* current fault (/proc) */
	u_char	p_swlocks;		/* number of swap vnode locks held */
	u_int	p_flag;			/* flags defined below */
	o_uid_t	p_uid;			/* for binary compatibility - real user id */
	o_pid_t	p_oppid;		/* process id of parent*/

	/* the following pid fields declared o_pid_t are for
	** binary compatibility only. They will be removed in
	** the next release following SVR4.0.
	*/

	o_pid_t	p_opgrp;		/* name of process group leader */
	o_pid_t	p_opid;			/* unique process id*/
	struct	cred	*p_cred;	/* process credentials */
	struct sess	*p_sessp;	/* session information */
	int	p_pri;			/* scheduling priority */

	caddr_t	p_wchan;		/* wait addr for sleeping processes */

	struct	proc	*p_link;	/* forward link */
	struct	proc	*p_parent;	/* ptr to parent process */
	struct	proc	*p_child;	/* ptr to first child process */
	struct	proc	*p_sibling;	/* ptr to next sibling proc on chain */

	int	p_clktim;		/* time to alarm clock signal */

	caddr_t	p_brkbase;		/* base address of heap */
	u_int	p_brksize;		/* heap size in bytes */
	caddr_t p_stkbase;		/* base address of stack */
	int	p_stksize;		/* stack size in words */

	u_int	p_swrss;		/* resident set size before last swap */
	clock_t	p_utime;		/* user time, this process */
	clock_t	p_stime;		/* system time, this process */

	pte_t	p_ubptbl[USIZE + 7];	/* page table for the u-block */
	struct as *p_as;		/* process address space pointer */
	int 	p_wdata;		/* current wait return value */
	char	p_wcode;		/* current wait code */
	char	p_pollflag;		/* flags for use during poll(2) */
	u_char	p_cursig;		/* current signal */
	pid_t	p_epid;			/* effective pid; normally same as
					 * p_pid; for servers, the system that
					 * sent the msg
					 */
	sysid_t p_sysid;		/* normally same as sysid; for servers,
					 * the system that sent the msg
					 */
	struct	proc  *p_rlink;		/* linked list for server */
	int	p_srwchan;		/* RFS server awaiting work; sleep */
	struct	vnode *p_trace;		/* pointer to /proc vnode */
	k_sigset_t p_sig;		/* signals pending to this process */
	k_sigset_t p_sigmask;		/* tracing signal mask for /proc */
	k_sigset_t p_hold;		/* hold signal bit mask */
	k_sigset_t p_ignore;		/* ignore when generated */
	k_sigset_t p_siginfo;		/* gets signal info with signal */
	u_short	p_whystop;
	u_short	p_whatstop;
	struct  anon *p_ubanon[USIZE];  /* for driver obj compatibility */
	k_fltset_t	p_fltmask;	/* mask of traced faults (/proc) */
	struct	evpd	*p_evpdp;	/* ptr to events per-process data */

	struct timer  *p_italarm[2];	/* Ptrs to the alarms for the clocks
					 * measuring user  process virtual
					 * time and user+system process
					 * virtual time.
					 */
	struct proc *p_pglink;		/* process group hash chain link */
	clock_t	p_cutime;		/* sum of children's user time */
	clock_t	p_cstime;		/* sum of children's system time */
	short	p_aiocount;		/* # of async requests accepted */
	short	p_aiowcnt;		/* # of async operations currently
					 * in progress and the process is
					 * waiting for their completion
					 */
	id_t	p_cid;			/* scheduling class id */
	_VOID	*p_clproc;		/* per process class specific data */
	struct classfuncs *p_clfuncs;	/* ptr to structure containing
					 * class specific function ptrs
					 */
	struct sigqueue *p_sigqueue;	/* queued siginfo structures */
	struct sigqueue *p_curinfo;	/* siginfo for current signal */
	caddr_t *p_segacct;		/* segment accounting info */
	struct seguser *p_segu;		/* u segment */
	struct	vnode *p_exec;		/* pointer to a.out vnode */
	pid_t	p_ppid;			/* process id of parent*/
	struct pid *p_pidp;		/* process ID info */
	struct pid *p_pgidp;		/* process group ID info */
	struct sd *p_sdp;		/* pointer to XENIX shared data */
	struct proc *p_next;		/* active chain link */
	struct proc *p_nextofkin;	/* gets accounting info at exit */
	struct proc *p_orphan;
	struct proc *p_nextorph;
} proc_t;

/* active process chain */

extern proc_t *practive;

/* Well known processes */

extern proc_t *proc_sched;		/* memory scheduler */
extern proc_t *proc_init;		/* init */
extern proc_t *proc_pageout;		/* pageout daemon */
extern proc_t *proc_bdflush;		/* block cache flush daemon */

/* process ID info */

struct pid {
	unsigned int pid_prinactive :1;
	unsigned int pid_pgorphaned :1;
	unsigned int pid_ref :6;
	unsigned int pid_prslot :24;
	pid_t pid_id;
	struct proc *pid_pglink;
	struct pid *pid_link;
};

extern struct pid pid0;

#define p_pgrp p_pgidp->pid_id
#define p_pid  p_pidp->pid_id
#define p_slot p_pidp->pid_prslot
#define p_detached p_pidp->pid_pgorphaned

#define PID_HOLD(pidp)	(++(pidp)->pid_ref)
#define PID_RELE(pidp)	((pidp)->pid_ref>1 ? --(pidp)->pid_ref : pid_rele(pidp))

/* stat codes */

#define	SSLEEP	1		/* awaiting an event */
#define	SRUN	2		/* running */
#define	SZOMB	3		/* process terminated but not waited for */
#define	SSTOP	4		/* process stopped by debugger */
#define	SIDL	5		/* intermediate state in process creation */
#define	SONPROC	6		/* process is being run on a processor */
#define SXBRK	7		/* process being xswapped */

/* flag codes */

#define	SSYS	0x00000001	/* system (resident) process */
#define	STRC	0x00000002	/* process is being traced */
#define	SPRWAKE	0x00000004	/* debugger is waiting to prlock() */
#define	SNWAKE	0x00000008	/* process cannot be awakened by a signal */
#define	SLOAD	0x00000010	/* in core */
#define	SLOCK   0x00000020	/* process cannot be swapped */
#define SLKDONE	0x00000040	/* NFS record-locking has been done */
#define	SPOLL	0x00000080	/* process in stream poll */
#define	SPRSTOP	0x00000100	/* process is being stopped via /proc */
#define	SPROCTR	0x00000200	/* signal or syscall tracing via /proc */
#define	SPROCIO	0x00000400	/* doing I/O via /proc, so don't run */
#define	SPRFORK	0x00000800	/* child inherits /proc tracing flags */
#define	SPROPEN	0x00001000	/* process is open via /proc */
#define	SULOAD  0x00002000	/* u-block in core */
#define	SRUNLCL	0x00004000	/* set process running on last /proc close */
#define	SNOSTOP	0x00008000	/* proc asleep, stop not allowed */
#define	SPTRX	0x00010000	/* process is exiting via ptrace(2) */
#define	SASLEEP	0x00020000	/* proc is stopped within a call to sleep() */
#define	SUSWAP	0x00040000	/* u-block is being swapped in or out */
#define	SUWANT	0x00080000	/* waiting for u-block swap to complete */
#define SEXECED 0x00100000	/* this process has execed */
#define SSWLOCKS 0x00200000 	/* process has swap locks */
#define SXSTART 0x00400000 	/* setrun() by SIGCONT or ptrace() */
#define SPSTART 0x00800000 	/* setrun() by /proc */
#define SJCTL	0x01000000	/* SIGCLD sent when children stop/continue */
#define SNOWAIT 0x02000000	/* children never become zombies */
#define SVFORK	0x04000000	/* process resulted from vfork */
#define SVFDONE 0x08000000	/* vfork child releasing parent as */

/* pollflags */

#define SINPOLL		0x01	/* process in poll */
#define SPOLLTIME	0x02	/* poll timeout pending */

/*
 * The following macro is required because a page table must
 * be on a 32 byte boundary and it may not be aligned properly
 * in the proc structure.
 */
#define	ubptbl(PP)	((((unsigned long)(PP)->p_ubptbl + 0x1F)) & ~0x1F)

/* Flags for newproc() */

#define NP_FAILOK	0x01	/* don't panic if cannot create process */
#define NP_NOLAST	0x02	/* don't use last process slot */
#define	NP_SYSPROC	0x04	/* system (resident) process */
#define	NP_INIT		0x08	/* this is init process */
#define NP_VFORK	0x10	/* share address space - vfork */
#define NP_SHARE	0x20	/* share address space - asyncio */

/* Macro to convert proc table pointers to user block pointers */
#define PTOU(p) ((struct user *)((p)->p_segu))

/* Macro to reduce unnecessary calls to issig() */

#define	ISSIG(p, why) \
  (((p)->p_cursig || !sigisempty(&(p)->p_sig) || ((p)->p_flag & SPRSTOP)) \
    && issig(why))

/* Reasons for calling issig() */

#define	FORREAL		0	/* Usual side-effects */
#define	JUSTLOOKING	1	/* Don't stop the process */

#ifdef _KERNEL

#ifdef __STDC__

/* process management functions */
extern caddr_t findvaddr(proc_t *);
extern paddr_t vtop(caddr_t, proc_t *);
extern void pexit(void);
extern int newproc(int, pid_t *, int *);
extern void vfwait(pid_t);
extern void freeproc(proc_t *);
extern void setrun(proc_t *);
extern void unsleep(proc_t *);
extern void exit(int, int);
extern void relvm(proc_t *);

extern void sigcld(proc_t *);
extern void psig(void);
extern int stop(proc_t *, int, int, int);
extern int issig(int);
extern void psignal(proc_t *, int);
extern void sigtoproc(proc_t *, int, int);

void pid_setmin(void);
pid_t pid_assign(int, proc_t **);
int pid_rele(struct pid *);
void pid_exit(proc_t *);
proc_t *prfind(pid_t);
proc_t *pgfind(pid_t);
void pid_init(void);
proc_t *pid_entry(int);
int pid_slot(proc_t *);
void signal(pid_t, int);
void prsignal(struct pid *, int);

void pgsignal(struct pid *, int);
void pgjoin(proc_t *, struct pid *);
void pgcreate(proc_t *);
void pgexit(proc_t *);
void pgdetach(proc_t *);
int pgmembers(pid_t);

#else

extern caddr_t findvaddr();
extern paddr_t vtop();
extern void pexit();
extern int newproc();
extern void vfwait();
extern void freeproc();
extern void setrun();
extern void unsleep();
extern void exit();
extern void relvm();

extern void sigcld();
extern void psig();
extern int stop();
extern int issig();
extern void psignal();
extern void sigtoproc();

void pid_setmin();
pid_t pid_assign();
int pid_rele();
void pid_exit();
proc_t *prfind();
proc_t *pgfind();
void pid_init();
proc_t *pid_entry();
int pid_slot();
void signal();
void prsignal();

void pgsignal();
void pgjoin();
void pgcreate();
void pgexit();
void pgdetach();
int pgmembers();

#endif	/* __STDC__ */

#endif	/* _KERNEL */

#endif	/* _SYS_PROC_H */
