#	Copyright (c) 1988 AT&T
#	All Rights Reserved 
#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any 
#	actual or intended publication of such source code.

# C library -- acct
.ident	"@(#)libc-m32:sys/acct.s	1.6"

	.set	__acct,51*8

	.globl  _cerror

_fwdef_(`acct'):
	MCOUNT
	MOVW	&4,%r0
	MOVW	&__acct,%r1
	GATE
	jgeu 	.noerror
	jmp 	_cerror
.noerror:
	CLRW	%r0
	RET
