/**
 * @file syscall_entry.S
 * Entry point for handling syscall exceptions.
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <mips.h>

.globl syscall_entry

/**
 * @fn exchandler syscall_entry(void)
 *
 * Handler for syscall exceptions.  Syscall number will be loaded into
 * register v0 and arguments should follow standard conventions.  Return
 * values should be loaded into v0 and v1.
 */
syscall_entry:
	.func syscall_entry
	addiu   sp, sp, -64      /* increase stack */
	mfc0    k0, CP0_EPC      /* get EPC */
	sw      ra, 60(sp)       /* store return address */
	sw      k0, 56(sp)       /* store EPC */

	sw      a0, 64(sp)       /* store arguments in safe positions */
	sw      a1, 68(sp)
	sw      a2, 72(sp)
	sw      a3, 76(sp)

	sw      s0, 16(sp)       /* store callee save registers */
	sw      s1, 20(sp)
	sw      s2, 24(sp)
	sw      s3, 28(sp)
	sw      s4, 32(sp)
	sw      s5, 36(sp)
	sw      s6, 40(sp)
	sw      s7, 44(sp)
	sw      s8, 48(sp)
	sw      s9, 52(sp)

	move    a0, v0           /* store code of syscall */
	la      a1, 64(sp)       /* load pointer to arguments */
	jal     syscall_dispatch /* handle system call */

	lw      k0, 56(sp)       /* Load up the exception PC */
	addiu   k0, k0, 4        /* System call was handled, move on */
	mtc0    k0, CP0_EPC      /* Store updated EPC address */

	lw      a0, 64(sp)       /* load arguments back to registers */
	lw      a1, 68(sp)
	lw      a2, 72(sp)
	lw      a3, 76(sp)

	lw      s0, 16(sp)       /* store callee save registers */
	lw      s1, 20(sp)
	lw      s2, 24(sp)
	lw      s3, 28(sp)
	lw      s4, 32(sp)
	lw      s5, 36(sp)
	lw      s6, 40(sp)
	lw      s7, 44(sp)
	lw      s8, 48(sp)
	lw      s9, 52(sp)

	lw      ra, 60(sp)       /* restore return address */
	addiu   sp, sp, 64       /* collapse stack */
	eret                     /* return to user mode */
	nop
	.endfunc
