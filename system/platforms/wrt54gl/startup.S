/**
 * @file     startup.S
 * @provides _startup, copyhandler, clearvector.
 * This is where kernel execution begins once loader hands off.  The boot
 * loader flushes caches, copies this kernel's interrupt handler to proper
 * location in memory and hands off control.
 *
 * $Id: startup.S 2103 2009-10-27 01:43:40Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <mips.h>
#include <interrupt.h>

#define NULLSTK      8192       /* Safe size for NULLSTK           */

.text
	.align 4
	.globl _startup

/**
 * _startup sets up the stack pointer and clears the stack for the
 * null process.  Additionally, the BSS (uninitialized data) section
 * is cleared prior to calling the null process.
 */
 	.ent _startup
_startup:
	/* Copy IRQ transfer handler to reserved memory location      */
	la    a0, IRQ_ADDR         /* Destination address             */
	la    a1, intdispatch      /* Starting address                */
	la    a2, (32*4)           /* Length of vector                */
	jal   copyhandler

	/* Clear Xinu-defined trap and interrupt vectors              */
	la    a0, TRAPVEC_ADDR
	la    a1, IRQVEC_END
	jal   clearvector
	
	/* Clear interrupt related registers in the coprocessor       */	
	mtc0  zero, CP0_STATUS     /* Clear interrupt masks           */
	mtc0  zero, CP0_CAUSE      /* Clear interrupt cause register  */

	/* Set up Stack segment (see function summary)                    */
 	li    s1, NULLSTK     /* Stack is NULLSTK bytes                   */
	la	  a0, _end
 	addu  s1, s1, a0      /* Find top of stack (_end + NULLSTK)       */

	/* Word align the top of the stack                                */
	subu  s1, s1, 1
	srl   s1, 4
	sll   s1, 4
	
	/* Initialize the stack and frame pointers                        */
	move  sp, s1
	move  fp, s1
	
	/* Zero out NULLSTK space below new stack pointer                 */
	subu  a1, s1, a0  
	jal   bzero 
	/* Note: bzero routine must be optimized leaf function without    */
	/* activation record, or this will zero its own return address.   */

	/* Clear the BSS section.                                         */
	la    a0, _bss
	la    a1, _end
	sub   a1, a1, a0
	jal   bzero

	/* Store processor ID                                             */
	mfc0  v0, CP0_PRID
	la    t0, cpuid
	sw    v0, 0(t0)

	/* Store bottom of the heap                                       */
	la    t0, memheap
	sw    s1, 0(t0)

	j     nulluser          /* start the null user process            */
 	.end _startup

/**
 * @fn void copyhandler(int *dst, int *src, uint bytes)
 * Copy text (code) from source to detination (in word-size chunks).
 * @param dst   location to store the source code
 * @param src   location holding the source code
 * @param bytes size of source code to copy
 */
	.ent copyhandler
copyhandler:
	lw    v0, 0(a1)
	sw    v0, 0(a0)
	addiu a1, a1, 4
	addiu a0, a0, 4
	addiu a2, a2, -4
	bgtz  a2, copyhandler
	jr    ra
	.end copyhandler

/**
 * @fn void clearvector(int *dstBegin, int *dstEnd)
 * Zero memory from dstBegin to dstEnd (non-inclusive).	
 * @param dstBegin start of the memory area of zero
 * @param dstEnd   end of the memory area to zero
 */
	.ent clearvector
clearvector:
	sw    zero, 0(a0)
	addiu a0, a0, 4
	blt   a0, a1, clearvector
	jr    ra
	.end clearvector
