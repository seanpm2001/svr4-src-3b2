/*	Copyright (c) 1984, 1986, 1987, 1988, 1989 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)kernel:ml/kpcbs.c	1.3"

/*
 *  Kernel fixed process control blocks
 */

#include "sys/types.h"
#include "sys/psw.h"
#include "sys/pcb.h"
#include "sys/param.h"


int	krnl_isp[19];
extern void int_null();
int kstkF[ISTKSZ];

struct kpcb kpcb_null ={{ KPSW15, int_null, kstkF },
			ZPSW, 0, 0, kstkF, &kstkF[ISTKSZ],
			{0,0,0,0,0,0,0,0,0,0,0}, 0 };

extern void int_px();
int kstk8[QSTKSZ];

struct kpcb kpcb_px ={{ KPSW8, int_px, kstk8 },
			ZPSW, 0, 0, kstk8, &kstk8[QSTKSZ],
			{0,0,0,0,0,0,0,0,0,0,0}, 0 };

extern void int_sx();
int	kstkSX[ISTKSZ];

struct kpcb kpcb_sx ={{ KPSW8, int_sx, kstkSX },
			ZPSW, 0, 0, kstkSX, &kstkSX[ISTKSZ],
			{0,0,0,0,0,0,0,0,0,0,0}, 0 };

extern void ps_swtch();

struct kpcb kpcb_pswtch ={{ KPSW8, ps_swtch, kstk8 },
				ZPSW, 0, 0, kstk8, &kstk8[QSTKSZ],
				{0,0,0,0,0,0,0,0,0,0,0}, 0 };

/*
 * The initial stack is gratuitous here as kpcb_qrun is not 
 * brought in for an interrupt.
 */
extern void queuerun();
int kstkQR[QSTKSZ];

struct kpcb kpcb_qrun ={{ KPSW15, queuerun, kstkQR},
			    ZPSW, 0, 0, kstkQR, &kstkQR[QSTKSZ],
			    {0,0,0,0,0,0,0,0,0,0,0}, 0};

extern void int_L9();
int kstkA[ISTKSZ];

struct kpcb kpcb_L9 ={{ KPSW10, int_L9, kstkA },
			ZPSW, 0, 0, kstkA, &kstkA[ISTKSZ],
			{0,0,0,0,0,0,0,0,0,0,0}, 0 };

extern void int_L15();

struct kpcb kpcb_L15 ={{ KPSW15, int_L15, kstkF },
			ZPSW, 0, 0, kstkF, &kstkF[ISTKSZ],
			{0,0,0,0,0,0,0,0,0,0,0}, 0 };
