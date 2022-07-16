#	Copyright (c) 1988 AT&T
#	All Rights Reserved 
#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any 
#	actual or intended publication of such source code.

# C library -- plock
.ident	"@(#)libc-m32:sys/plock.s	1.6"

# error = plock(op)

	.set	__plock,45*8

	.globl	_cerror

_fwdef_(`plock'):
	MCOUNT
	MOVW	&4,%r0
	MOVW	&__plock,%r1
	GATE
	jgeu	.noerror
	jmp	_cerror
.noerror:
	RET
