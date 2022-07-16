/*	Copyright (c) 1988 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)ld:m32/machdep.h	1.14"
/*
 * Machine dependent declarations
 */

#ifndef	MACHDEP_DOT_H
#define	MACHDEP_DOT_H

#include	"sys/elf_M32.h"

/*
 * Make machine class dependent data types transparent to the common code
 */

#define	Word	Elf32_Word
#define Sword	Elf32_Sword
#define Half	Elf32_Half
#define	Addr	Elf32_Addr
#define	Off	Elf32_Off
#define Byte	unsigned char

#define Ehdr	Elf32_Ehdr
#define Shdr	Elf32_Shdr
#define	Sym	Elf32_Sym
#define	Rel	Elf32_Rel
#define	Phdr	Elf32_Phdr
#define Dyn	Elf32_Dyn

/*
 * Make machine class dependent functions transparent to the common code
 */

#define	ELF_R_TYPE	ELF32_R_TYPE
#define	ELF_R_INFO	ELF32_R_INFO
#define	ELF_R_SYM	ELF32_R_SYM
#define	ELF_ST_BIND	ELF32_ST_BIND
#define	ELF_ST_TYPE	ELF32_ST_TYPE
#define	ELF_ST_INFO	ELF32_ST_INFO
#define	elf_fsize	elf32_fsize
#define	elf_getehdr	elf32_getehdr
#define	elf_newehdr	elf32_newehdr
#define	elf_getphdr	elf32_getphdr
#define	elf_newphdr	elf32_newphdr
#define	elf_getshdr	elf32_getshdr
#define elf_xlatetof	elf32_xlatetof
#define	elf_xlatetom	elf32_xlatetom

/*
 * Other machine dependent entities
 */

#define	BYTE_ORDER	ELFDATA2MSB

#define	MACHINE_TYPE	EM_M32

#define MACHINE_CLASS	ELFCLASS32

#define	FIRSTSEG_ORIGIN	(Addr)0x80000000

#define LIBFIRSTSEG_ORIGIN (Addr)0x80800000

#ifdef	ELF_M32_MAXPGSZ
#define	CHUNK_SIZE	ELF_M32_MAXPGSZ
#else
#define	CHUNK_SIZE	0x2000
#endif

#define WORD_ALIGN	ELF32_FSZ_WORD

/* sizes of got and plt entries */
#define PLTENTSZ	18
#define PLT_INST_SZ	6
#define GOTENTSZ	ELF32_FSZ_WORD

#define	SETEHDR_FLAGS()	{ ehdr_flags |= cur_file_ehdr->e_flags & EF_M32_MAU; }

/* relocation type */
#define DT_REL_TYPE	DT_REL		/* relocation without addends */
#define SHT_REL_TYPE	SHT_REL		/* section header type */
#define ELF_T_REL_TYPE	ELF_T_REL	/* rawbits type */
	/* permissions on plt shdr */
#define PLT_SHF_PERM	SHF_ALLOC | SHF_EXECINSTR

/* text section padding filling */
#define	NOP	0x70

#define	FILL_TEXT()	{ \
	Elf_Data *d; \
	static unsigned char nops[] = { \
		NOP,NOP,NOP,NOP,NOP,NOP,NOP,NOP, \
		NOP,NOP,NOP,NOP,NOP,NOP,NOP,NOP \
	}; \
	static unsigned char *nbuf = nops; \
	static unsigned nsize = sizeof(nops); \
	if (fill_bytes > nsize - 4) { \
		nsize = ROUND(fill_bytes, 4) + 4; \
		nbuf = (unsigned char *) malloc(nsize); \
		(void) memset(nbuf, NOP, nsize); \
	} \
	d = my_elf_newdata(scn); \
	d->d_buf = (char *) nbuf; d->d_type = ELF_T_BYTE; \
	d->d_size = fill_bytes; d->d_align = 1; d->d_version = EV_CURRENT; \
}

#endif	/* MACHDEP_DOT_H */
