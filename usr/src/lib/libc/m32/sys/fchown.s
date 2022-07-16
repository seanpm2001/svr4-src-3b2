#	Copyright (c) 1988 AT&T
#	All Rights Reserved 
#	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T
#	The copyright notice above does not evidence any 
#	actual or intended publication of such source code.

.ident	"@(#)libc-m32:sys/fchown.s	1.1"

# error = fchown(fd,owner,group)

	.set	__fchown,94*8

	.globl	_cerror

_fwdef_(`fchown'):
	MCOUNT
	MOVW	&4,%r0
	MOVW	&__fchown,%r1
	GATE
	jgeu 	.noerror
	jmp 	_cerror
.noerror:
	CLRW	%r0
	RET
