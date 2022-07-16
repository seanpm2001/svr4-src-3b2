/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)fs:fs/proc/prvnops.c	1.24"
#include "sys/types.h"
#include "sys/param.h"
#include "sys/time.h"
#include "sys/cred.h"
#include "sys/debug.h"
#include "sys/dirent.h"
#include "sys/errno.h"
#include "sys/file.h"
#include "sys/immu.h"
#include "sys/inline.h"
#include "sys/kmem.h"
#include "sys/pathname.h"
#include "sys/proc.h"
#include "sys/signal.h"
#include "sys/stat.h"
#include "sys/sysmacros.h"
#include "sys/systm.h"
#include "sys/uio.h"
#include "sys/var.h"
#include "sys/vfs.h"
#include "sys/vnode.h"
#include "sys/mode.h"

#include "sys/psw.h"
#include "sys/pcb.h"
#include "sys/sbd.h"
#include "sys/user.h"

#include "sys/fault.h"
#include "sys/syscall.h"
#include "sys/procfs.h"

#include "fs/fs_subr.h"

#include "vm/rm.h"

#include "prdata.h"

/* /proc vnode operations vector */

STATIC int	propen(), prclose(), prread(), prwrite();
STATIC int	prgetattr(), praccess(), prlookup(), prreaddir();
STATIC int	prfsync(), prseek();

STATIC void	prinactive();

extern int	prioctl();	/* in prioctl.c */

struct vnodeops prvnodeops = {
	propen,
	prclose,
	prread,
	prwrite,
	prioctl,
	fs_setfl,
	prgetattr,
	fs_nosys,	/* setattr */
	praccess,
	prlookup,
	fs_nosys,	/* create */
	fs_nosys,	/* remove */
	fs_nosys,	/* link */
	fs_nosys,	/* rename */
	fs_nosys,	/* mkdir */
	fs_nosys,	/* rmdir */
	prreaddir,
	fs_nosys,	/* symlink */
	fs_nosys,	/* readlink */
	prfsync,
	prinactive,
	fs_nosys,	/* fid */
	fs_rwlock,
	fs_rwunlock,
	prseek,
	fs_cmp,
	fs_nosys,	/* frlock */
	fs_nosys,	/* space */
	fs_nosys,	/* realvp */
	fs_nosys,	/* getpage */
	fs_nosys,	/* putpage */
	fs_nosys,	/* map */
	fs_nosys,	/* addmap */
	fs_nosys,	/* delmap */
	fs_poll,
	fs_nosys,	/* dump */
	fs_nosys,	/* filler */
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
	fs_nosys,
};

STATIC struct vnode *prget();
STATIC struct prnode *prfreelist = NULL;	/* Free list of /proc vnodes */

/*
 * Directory characteristics (patterned after the s5 file system).
 */
#define	PRROOTINO	2

#define	PRDIRSIZE	14
struct prdirect {
	u_short	d_ino;
	char	d_name[PRDIRSIZE];
};

#define	PRSDSIZE	(sizeof(struct prdirect))

STATIC int
propen(vpp, flag, cr)
	struct vnode **vpp;
	register int flag;
	struct cred *cr;
{
	register struct vnode *vp = *vpp;
	register struct prnode *pnp = VTOP(vp);
	register struct proc *p = pnp->pr_proc;
	register int error = 0;
	int locked;

	/*
	 * If the process exists, lock it and the entire /proc mechanism now.
	 * Otherwise we have a race condition with prclose().
	 */
	locked = (vp->v_type == VREG && (error = prlock(pnp, ZYES, 1)) == 0);
	if (error)
		return error;

	if (pnp->pr_flags & PRINVAL) {
		error = EAGAIN;
		goto out;
	}

	/*
	 * Maintain a count of opens for write.  Allow exactly one
	 * O_RDWR|O_EXCL request and fail subsequent ones (even for
	 * the super-user).  Once an exclusive open is in effect,
	 * fail subsequent attempts to open for writing (except for
	 * the super-user).
	 */
	if (flag & FWRITE) {
		if (flag & FEXCL) {
			if (pnp->pr_writers > 0) {
				error = EBUSY;
				goto out;
			}
			pnp->pr_flags |= PREXCL;
		} else if (pnp->pr_flags & PREXCL) {
			ASSERT(pnp->pr_writers > 0);
			if (cr->cr_uid) {
				error = EBUSY;
				goto out;
			}
		}
		pnp->pr_writers++;
	}

	/*
	 * Keep a count of opens so that we can identify the last close.
	 * The vnode reference count (v_count) is unsuitable for this
	 * because references are generated by other operations in
	 * addition to open and close.
	 */
	pnp->pr_opens++;
	if (locked) {
		ASSERT(p != NULL && p->p_trace == vp);
		p->p_flag |= SPROPEN;
	}
out:
	if (locked)
		prunlock(pnp);

	return error;
}

/* ARGSUSED */
STATIC int
prclose(vp, flag, count, offset, cr)
	register struct vnode *vp;
	int flag;
	int count;
	off_t offset;
	struct cred *cr;
{
	register struct prnode *pnp = VTOP(vp);
	register struct proc *p = pnp->pr_proc;
	register struct vnode *pvp;
	user_t *up;
	int locked;

	/*
	 * There is nothing to do until the last close
	 * of the file table entry.
	 */
	if (count > 1)
		return 0;

	/*
	 * If the process exists, lock it and the entire /proc mechanism now.
	 * Otherwise we have a race condition with propen().
	 */
	locked = (vp->v_type == VREG && prlock(pnp, ZYES, 1) == 0);

	/*
	 * When the last reference to a writable file descriptor goes
	 * away, decrement the count of writers.  When the count of
	 * writers drops to zero, clear the "exclusive-use" flag.
	 * Anomaly: we can't distinguish multiple writers enough to
	 * tell which one originally requested exclusive-use, so once
	 * exclusive-use is set it stays on until all writers have
	 * gone away.
	 */
	--pnp->pr_opens;
	if ((flag & FWRITE) && --pnp->pr_writers == 0)
		pnp->pr_flags &= ~PREXCL;

	/*
	 * If there is no process, there is nothing more to do.
	 */
	if (!locked)
		return 0;

	/*
	 * On last close of all writable file descriptors,
	 * perform run-on-last-close logic.
	 */
	for (pvp = p->p_trace; pvp != NULL; pvp = VTOP(pvp)->pr_vnext)
		if (VTOP(pvp)->pr_writers)
			break;
	if (pvp == NULL && (p->p_flag & SRUNLCL)) {
		/*
		 * If any tracing flags are set, clear them.
		 */
		if ((p->p_flag & SPROCTR) && p->p_stat != SZOMB) {
			up = prumap(p);
			premptyset(&up->u_entrymask);
			premptyset(&up->u_exitmask);
			up->u_systrap = 0;
			prunmap(p);
		}
		premptyset(&p->p_sigmask);
		premptyset(&p->p_fltmask);
		p->p_flag &= ~(SPRSTOP|SRUNLCL|SPROCTR|SPRFORK);
		/*
		 * If process is stopped by /proc, set it running.
		 */
		if (p->p_stat == SSTOP && (p->p_flag & SPSTART) == 0) {
			p->p_flag |= SPSTART;
			setrun(p);
		}
	}

	/*
	 * On last close of all /proc file descriptors,
	 * reset the process's proc-open flag.
	 */
	for (pvp = p->p_trace; pvp != NULL; pvp = VTOP(pvp)->pr_vnext)
		if (VTOP(pvp)->pr_opens)
			break;
	if (pvp == NULL)
		p->p_flag &= ~SPROPEN;

	prunlock(pnp);

	return 0;
}

/* ARGSUSED */
STATIC int
prread(vp, uiop, ioflag, cr)
	register struct vnode *vp;
	register struct uio *uiop;
	int ioflag;
	struct cred *cr;
{
	static struct prdirect dotbuf[] = {
		{ PRROOTINO, "."  },
		{ PRROOTINO, ".." }
	};
	struct prdirect dirbuf;
	register int i, n, j;
	int minproc, maxproc, modoff;
	proc_t *p;
	register struct prnode *pnp = VTOP(vp);
	int error = 0;

	if (vp->v_type == VDIR) {
		/*
		 * Fake up ".", "..", and the /proc directory entries.
		 */
		if (uiop->uio_offset < 0
		  || uiop->uio_offset >= (v.v_proc + 2) * PRSDSIZE
		  || uiop->uio_resid <= 0)
			return 0;
		if (uiop->uio_offset < 2*PRSDSIZE) {
			error = uiomove((caddr_t)dotbuf + uiop->uio_offset,
			  min(uiop->uio_resid, 2*PRSDSIZE - uiop->uio_offset),
			  UIO_READ, uiop);
			if (uiop->uio_resid <= 0 || error)
				return error;
		}
		minproc = (uiop->uio_offset - 2*PRSDSIZE)/PRSDSIZE;
		maxproc = (uiop->uio_offset + uiop->uio_resid - 1)/PRSDSIZE;
		modoff = uiop->uio_offset % PRSDSIZE;
		for (j = 0; j < PRDIRSIZE; j++)
			dirbuf.d_name[j] = '\0';
		for (i = minproc; i < min(maxproc, v.v_proc); i++) {
			if ((p = pid_entry(i)) != NULL) {
				n = p->p_pid;
				dirbuf.d_ino = ptoi(n);
				for (j = PNSIZ-1; j >= 0; j--) {
					dirbuf.d_name[j] = n % 10 + '0';
					n /= 10;
				}
			} else {
				dirbuf.d_ino = 0;
				for (j = 0; j <= PNSIZ-1; j++)
					dirbuf.d_name[j] = '\0';
			}
			error = uiomove((caddr_t)&dirbuf + modoff,
			  min(uiop->uio_resid, PRSDSIZE - modoff),
			    UIO_READ, uiop);
			if (uiop->uio_resid <= 0 || error)
				return error;
			modoff = 0;
		}
	} else if ((error = prlock(pnp, ZNO, 0)) == 0) {
		if (pnp->pr_flags & PRINVAL)
			error = EAGAIN;
		else
			error = prusrio(pnp->pr_proc, UIO_READ, uiop);
		prunlock(pnp);
	}
	return error;
}

/* ARGSUSED */
STATIC int
prwrite(vp, uiop, ioflag, cr)
	register struct vnode *vp;
	register struct uio *uiop;
	int ioflag;
	struct cred *cr;
{
	register struct prnode *pnp = VTOP(vp);
	int error;

	if (vp->v_type == VDIR)
		error = EISDIR;
	else if ((error = prlock(pnp, ZNO, 0)) == 0) {
		if (pnp->pr_flags & PRINVAL)
			error = EAGAIN;
		else
			error = prusrio(pnp->pr_proc, UIO_WRITE, uiop);
		prunlock(pnp);
	}
	return error;
}

/* prioctl in prioctl.c */

/* ARGSUSED */
STATIC int
prgetattr(vp, vap, flags, cr)
	register struct vnode *vp;
	register struct vattr *vap;
	int flags;
	struct cred *cr;
{
	register struct prnode *pnp = VTOP(vp);
	register proc_t *p;

	/*
	 * Return all the attributes.  Should be refined so that it
	 * returns only those asked for.
	 *
	 * Most of this is complete fakery anyway.
	 */
	if (pnp->pr_flags & PRINVAL)
		return EAGAIN;
	if (vp->v_type == VDIR) {
		vap->va_uid = 0;
		vap->va_gid = 0;
		vap->va_nlink = 2;
		vap->va_nodeid = PRROOTINO;
		vap->va_size = (v.v_proc + 2) * PRSDSIZE;
	} else if ((p = pnp->pr_proc) != NULL) {
		vap->va_uid = p->p_cred->cr_uid;
		vap->va_gid = p->p_cred->cr_gid;
		vap->va_nlink = 1;
		vap->va_nodeid = ptoi(p->p_pid);
		vap->va_size = rm_assize(p->p_as);
	} else
		return ENOENT;

	vap->va_type = vp->v_type;
	vap->va_mode = pnp->pr_mode;
	vap->va_fsid = procdev;
	vap->va_rdev = 0;
	vap->va_atime = vap->va_mtime = vap->va_ctime = hrestime;
	vap->va_blksize = 1024;
	vap->va_nblocks = btod(vap->va_size);
	vap->va_vcode = 0;
	return 0;
}

/* ARGSUSED */
STATIC int
praccess(vp, mode, flags, cr)
	register struct vnode *vp;
	register int mode;
	int flags;
	register struct cred *cr;
{
	register struct prnode *pnp = VTOP(vp);
	register proc_t *p;
	register int error;

	if (pnp->pr_flags & PRINVAL)
		return EAGAIN;
	if ((mode & VWRITE) && (vp->v_vfsp->vfs_flag & VFS_RDONLY))
		return EROFS;
	if (cr->cr_uid == 0)
		return 0;
	if (vp->v_type == VREG) {
		if ((p = pnp->pr_proc) == NULL || p->p_stat == SZOMB)
			return ENOENT;
		if (cr->cr_uid != p->p_cred->cr_ruid
		  || cr->cr_uid != p->p_cred->cr_suid
		  || cr->cr_gid != p->p_cred->cr_rgid
		  || cr->cr_gid != p->p_cred->cr_sgid)
			return EACCES;
		if (error = prisreadable(p, cr))
			return error;
	}
	if ((pnp->pr_mode & mode) == mode)
		return 0;
	return EACCES;
}

/* ARGSUSED */
STATIC int
prlookup(dp, comp, vpp, pnp, flags, rdir, cr)
	struct vnode *dp;
	register char *comp;
	struct vnode **vpp;
	struct pathname *pnp;
	int flags;
	struct vnode *rdir;
	struct cred *cr;
{
	register int n = 0;

	if (comp[0] == 0 || strcmp(comp, ".") == 0 || strcmp(comp, "..") == 0) {
		VN_HOLD(dp);
		*vpp = dp;
		return 0;
	}
	while (*comp) {
		if (*comp < '0' || *comp > '9')
			return ENOENT;
		n = 10 * n + *comp++ - '0';
	}
	*vpp = prget(n);
	return (*vpp == NULL) ? ENOENT : 0;
}

/*
 * Find or construct a vnode for the given pid.
 */
STATIC struct vnode *
prget(pid)
	int pid;
{
	register proc_t *p;
	register struct prnode *pnp;
	register struct vnode *vp;

again:
	if ((p = prfind(pid)) == NULL || p->p_stat == SIDL)
		return NULL;
	if (p->p_trace && (VTOP(p->p_trace)->pr_flags & PRINVAL) == 0) {
		VN_HOLD(p->p_trace);
		return p->p_trace;
	}
	/*
	 * New /proc vnode required; allocate it and fill in all the fields.
	 */
	if (prfreelist) {
		pnp = prfreelist;
		prfreelist = pnp->pr_free;
	} else {
		pnp = (prnode_t *)kmem_zalloc(sizeof(prnode_t), KM_SLEEP);
		/*
		 * We might have slept in the allocation routine.
		 * free the vnode and loop around to try again.
		 */
		ASSERT(pnp != NULL);
		pnp->pr_free = prfreelist;
		prfreelist = pnp;
		goto again;
	}

	/*
	 * We must not sleep in this section of code.
	 * It is not reentrant.
	 */
	vp = &pnp->pr_vnode;
	vp->v_type = VREG;
	vp->v_vfsp = procvfs;
	vp->v_vfsmountedhere = NULL;
	vp->v_op = &prvnodeops;
	vp->v_count = 1;
	vp->v_data = (caddr_t) pnp;
	vp->v_flag = VNOMAP;
	pnp->pr_mode = 0600;	/* R/W only by owner */
	pnp->pr_proc = p;
	pnp->pr_free = NULL;
	pnp->pr_opens = 0;
	pnp->pr_writers = 0;
	pnp->pr_flags = 0;
	/*
	 * If there is a left-over invalid /proc vnode, link it to
	 * the new vnode.
	 */
	ASSERT(p->p_trace == NULL || (VTOP(p->p_trace)->pr_flags & PRINVAL));
	pnp->pr_vnext = p->p_trace;
	p->p_trace = vp;
	return vp;
}

/* ARGSUSED */
STATIC int
prreaddir(vp, uiop, cr, eofp)
	struct vnode *vp;
	register struct uio *uiop;
	struct cred *cr;
	int *eofp;
{
	/* bp holds one dirent structure */
	char bp[round(sizeof(struct dirent)-1+PNSIZ+1)];
	struct dirent *dirent = (struct dirent *) bp;
	int reclen;
	register int i, j, n;
	int oresid, dsize;
	off_t off;
	proc_t *p;

	if (uiop->uio_offset < 0 || uiop->uio_resid <= 0
	  || (uiop->uio_offset % PRSDSIZE) != 0)
		return ENOENT;
	dsize = (char *)dirent->d_name - (char *)dirent;
	oresid = uiop->uio_resid;
	/*
	 * Loop until user's request is satisfied or until all processes
	 * have been examined.
	 */
	for (; uiop->uio_resid > 0; uiop->uio_offset = off + PRSDSIZE) {
		if ((off = uiop->uio_offset) == 0) {	/* "." */
			dirent->d_ino = PRROOTINO;
			dirent->d_name[0] = '.';
			dirent->d_name[1] = '\0';
			reclen = dsize+1+1;
		} else if (off == PRSDSIZE) { /* ".." */
			dirent->d_ino = PRROOTINO;
			dirent->d_name[0] = '.';
			dirent->d_name[1] = '.';
			dirent->d_name[2] = '\0';
			reclen = dsize+2+1;
		} else {
			/*
			 * Stop when entire proc table has been examined.
			 */
			if ((i = (off-2*PRSDSIZE)/PRSDSIZE) >= v.v_proc)
				break;
			if ((p = pid_entry(i)) == NULL)
				continue;
			n = p->p_pid;
			dirent->d_ino = ptoi(n);
			for (j = PNSIZ-1; j >= 0; j--) {
				dirent->d_name[j] = n % 10 + '0';
				n /= 10;
			}
			dirent->d_name[PNSIZ] = '\0';
			reclen = dsize+PNSIZ+1;
		}
		dirent->d_off = uiop->uio_offset + PRSDSIZE;
		/*
		 * Pad to nearest word boundary (if necessary).
		 */
		for (i = reclen; i < round(reclen); i++)
			dirent->d_name[i-dsize] = '\0';
		dirent->d_reclen = reclen = round(reclen);
		if (reclen > uiop->uio_resid) {
			/*
			 * Error if no entries have been returned yet.
			 */
			if (uiop->uio_resid == oresid)
				return EINVAL;
			break;
		}
		/*
		 * uiomove() updates both resid and offset by the same
		 * amount.  But we want offset to change in increments
		 * of PRSDSIZE, which is different from the number of bytes
		 * being returned to the user.  So we set uio_offset
		 * separately, ignoring what uiomove() does.
		 */
		if (uiomove((caddr_t) dirent, reclen, UIO_READ, uiop))
			return EFAULT;
	}
	if (eofp)
		*eofp = ((uiop->uio_offset-2*PRSDSIZE)/PRSDSIZE >= v.v_proc);
	return 0;
}

/* ARGSUSED */
STATIC int
prfsync(vp, uiop, cr)
	struct vnode *vp;
	struct uio *uiop;
	struct cred *cr;
{
	return 0;
}

/* ARGSUSED */
STATIC void
prinactive(vp, cr)
	register struct vnode *vp;
	struct cred *cr;
{
	register struct prnode *pnp;
	register proc_t *p;

	if (vp->v_type == VDIR)
		return;
	pnp = VTOP(vp);
	ASSERT(pnp->pr_opens == 0);

	/*
	 * Reset the process's vnode pointer.  Be careful about invalid
	 * vnodes to make sure the links are maintained properly.
	 */
	if ((p = pnp->pr_proc) != NULL) {
		register struct vnode *pvp = p->p_trace;
		ASSERT(pvp != NULL);

		if (vp == pvp)
			p->p_trace = pnp->pr_vnext;
		else {
			while (VTOP(pvp)->pr_vnext != vp) {
				pvp = VTOP(pvp)->pr_vnext;
				ASSERT(pvp != NULL);
			}
			VTOP(pvp)->pr_vnext = pnp->pr_vnext;
		}
	}
	pnp->pr_vnext = NULL;
	pnp->pr_free = prfreelist;
	prfreelist = pnp;
}

/* ARGSUSED */
STATIC int
prseek(vp, ooff, noffp)
	struct vnode *vp;
	off_t ooff;
	off_t *noffp;
{
	return 0;
}
