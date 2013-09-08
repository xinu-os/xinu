/**
 * @file start.S
 * This is where the firmware environment begins to execute code.
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <mips.h>
#include <interrupt.h>

.globl _start
	
/**
 * The firmware will load the image into temporary
 * memory via TFTP and will then jump to this (_start) label.
 * The boot loader (_start) flushes L1 instruction and data caches.
 */
_start:
	.func _start
	/* Clear and invalidate the L1 instruction and data caches    */
	jal   flushcache
	
	/* Pass control to Xinu kernel.                               */
	j     _startup
	.endfunc

/**
 * Intialize and flush the L1 instruction and data caches.
 */
flushcache:
	.func flushcache

	/* Intialize instruction cache */

	/*
	 * Register values for following code:	
	 * t0 = CP0_CONFIG
	 * t1 = CF_I* values
	 * t2 = index positions
	 * t3 = associativity
	 * t4 = number of lines
	 * t5 = line size
	 * t6 = I cache size
	 */
	
	/* Obtain Config1 register contents */
	mfc0  t0, CP0_CONFIG, 1

	/* Obtain CFG_IS */
	srl   t1, t0, CONFIG1_IS
	andi  t1, t1, CONFIG1_MASK
	
	/* Index positions = 64 * 2^(CFG_IS) */
	li    t2, 64
	sll   t2, t2, t1
	
	/* Obtain CFG_IA */
	srl   t1, t0, CONFIG1_IA
	andi  t1, t1, CONFIG1_MASK
	
	/* Associativity = CFG_IA + 1*/
	addiu t3, t1, 1
	
	/* Number of lines = Associativity * Index positions */
	multu t3, t2
	mflo  t4
	
	/* Obtain CFG_IL */
	srl   t1, t0, CONFIG1_IL
	andi  t1, t1, CONFIG1_MASK
	
	/* Line size = 2 * 2^(CFG_IL) */
	li    t5, 2
	sll   t5, t5, t1
		
	/* I cache size = Line size * # of lines */
	multu t5, t4
	mflo  t6

	/*
	 * Register values for following code:
	 * t0 = current address
	 * t1 = ending address
	 * t4 = number of lines
	 * t5 = line size
	 * t6 = I cache size
	 */	
	
	/* Clear icache tags to invalidate */
	mtc0  zero, CP0_TAGLO
	mtc0  zero, CP0_TAGHI

	li    t0, KSEG0_BASE
	addu  t1, t0, t6
1:	cache INDEX_STORE_TAG_I, 0(t0)
	addu  t0, t0, t5
	bne   t0, t1, 1b

	/* Fill icache once for data field parity */
	li    t0, KSEG0_BASE
	addu  t1, t0, t6
1:	cache FILL_I_CACHE, 0(t0)
	addu  t0, t0, t5
	bne   t0, t1, 1b

	/* Clear tags again--deemed prudent by some */
	li    t0, KSEG0_BASE
	addu  t1, t0, t6
1:	cache INDEX_STORE_TAG_I, 0(t0)
	addu  t0, t0, t5
	bne   t0, t1, 1b
	
	/* Intialize data cache */

	/*
	 * Register values for following code:
	 * t0 = CP0_CONFIG
	 * t1 = CF_D* values
	 * t2 = index positions
	 * t3 = associativity
	 * t4 = number of lines
	 * t5 = line size
	 * t6 = D cache size
	 */
	
	/* Obtain Config1 register contents */
	mfc0  t0, CP0_CONFIG, 1

	/* Obtain CFG_DS */
	srl   t1, t0, CONFIG1_DS
	andi  t1, t1, CONFIG1_MASK
	
	/* Index positions = 64 * 2^(CFG_DS) */
	li    t2, 64
	sll   t2, t2, t1
	
	/* Obtain CFG_DA */
	srl   t1, t0, CONFIG1_DA
	andi  t1, t1, CONFIG1_MASK
	
	/* Associativity = CFG_DA + 1*/
	addiu t3, t1, 1
	
	/* Number of lines = Associativity * Index positions */
	multu t3, t2
	mflo  t4
	
	/* Obtain CFG_DL */
	srl   t1, t0, CONFIG1_DL
	andi  t1, t1, CONFIG1_MASK
	
	/* Line size = 2 * 2^(CFG_DL) */
	li    t5, 2
	sll   t5, t5, t1
		
	/* D cache size = Line size * # of lines */
	multu t5, t4
	mflo  t6

	/*
	 * Register values for following code:
	 * t0 = current address
	 * t1 = ending address
	 * t4 = number of lines
	 * t5 = line size
	 * t6 = D cache size
	 */	
	
	/* Clear dcache tags to invalidate */
	li    t0, KSEG0_BASE
	addu  t1, t0, t6
1:	cache INDEX_STORE_TAG_I, 0(t0)
	addu  t0, t0, t5
	bne   t0, t1, 1b

	/* Load each line in dcache once */
	li    t0, KSEG0_BASE
	addu  t1, t0, t6
1:	lw    t4, 0(t0)
	addu  t0, t0, t5
	bne   t0, t1, 1b

	/* Clear tags again--deemed prudent by some */
	li    t0, KSEG0_BASE
	addu  t1, t0, t6
1:	cache INDEX_STORE_TAG_I, 0(t0)
	addu  t0, t0, t5
	bne   t0, t1, 1b

	jr    ra
	.endfunc

#define NULLSTK 8192

/**
 * _startup sets up the stack pointer and clears the stack for the
 * null process.  Additionally, the BSS (uninitialized data) section
 * is cleared prior to calling the null process.
 */
_startup:
	.func _startup
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
 	.endfunc

/**
 * @fn void copyhandler(int *dst, int *src, uint bytes)
 * Copy text (code) from source to detination (in word-size chunks).
 * @param dst   location to store the source code
 * @param src   location holding the source code
 * @param bytes size of source code to copy
 */
copyhandler:
	.func copyhandler
	lw    v0, 0(a1)
	sw    v0, 0(a0)
	addiu a1, a1, 4
	addiu a0, a0, 4
	addiu a2, a2, -4
	bgtz  a2, copyhandler
	jr    ra
	.endfunc

/**
 * @fn void memzero(int *dstBegin, int *dstEnd)
 * Zero memory from dstBegin to dstEnd (non-inclusive).	
 * @param dstBegin start of the memory area of zero
 * @param dstEnd   end of the memory area to zero
 */
memzero:
	.func memzero
	sw    zero, 0(a0)
	addiu a0, a0, 4
	blt   a0, a1, memzero
	jr    ra
	.endfunc
