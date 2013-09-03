/**
 * @file     startup.S
 * This is where kernel execution begins once loader hands off.  The boot
 * loader flushes caches, copies this kernel's interrupt handler to proper
 * location in memory and hands off control.
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <mips.h>
#include <interrupt.h>

#define NULLSTK      8192      /* Safe size for NULLSTK               */

.extern flash_size

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
	/* Bootloader passes flash size in a3                         */
	sw a3, flash_size
	/* Copy IRQ transfer handler to reserved memory location      */
	la    a0, IRQ_ADDR     /* Destination address                 */
	la    a1, intdispatch  /* Starting address                    */
	la    a2, (32*4)       /* Length of vector                    */
	jal   copyhandler

	/* Clear Xinu-defined trap and interrupt vectors              */
	la    a0, TRAPVEC_ADDR
	la    a1, IRQVEC_END
	jal   memzero
	
	/* Clear interrupt related registers in the coprocessor       */	
	mtc0  zero, CP0_STATUS /* Clear interrupt masks               */
	mtc0  zero, CP0_CAUSE  /* Clear interrupt cause register      */

	/* Clear and invalidate the L1 instruction and data caches    */
	jal   flushcache
		
	/* Set up Stack segment (see function summary)                */
 	li    s0, NULLSTK      /* Stack is NULLSTK bytes              */
	la    a0, _end
 	addu  s0, s0, a0       /* Find top of stack (_end + NULLSTK)  */

	/* Word align the top of the stack                            */
	subu  s1, s0, 1
	srl   s1, 4
	sll   s1, 4
	
	/* Initialize the stack and frame pointers                    */
	move  sp, s1
	move  fp, s1
	
	/* Zero out NULLSTK space below new stack pointer             */
	move  a1, s0
	jal   memzero 

	/* Clear the BSS section.                                     */
	la    a0, _bss
	la    a1, _end
	jal   memzero

	/* Store processor ID                                         */
	mfc0  v0, CP0_PRID
	la    t0, cpuid
	sw    v0, 0(t0)

	/* Store bottom of the heap                                   */
	la    t0, memheap
	sw    s0, 0(t0)

	j     nulluser         /* start the null user process         */
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
 * @fn void memzero(int *dstBegin, int *dstEnd)
 * Zero memory from dstBegin to dstEnd (non-inclusive).	
 * @param dstBegin start of the memory area of zero
 * @param dstEnd   end of the memory area to zero
 */
	.ent memzero
memzero:
	sw    zero, 0(a0)
	addiu a0, a0, 4
	blt   a0, a1, memzero
	jr    ra
	.end memzero
