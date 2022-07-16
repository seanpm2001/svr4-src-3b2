#	Copyright (c) 1988 AT&T
#	All Rights Reserved 
#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any 
#	actual or intended publication of such source code.

.ident	"@(#)libc-m32:sys/getgroups.s	1.1"
# ngroups = getgroups(gidsetsize, grouplist);

	.set	__getgroups,92*8

	.globl	_cerror

_fwdef_(`getgroups'):
	MCOUNT
	MOVW	&4,%r0
	MOVW	&__getgroups,%r1
	GATE
	jgeu	.noerror
	jmp	_cerror
.noerror:
	RET
