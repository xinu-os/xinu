/**
 * @file flushcache.S
 * @provides flushcache.
 *
 * The overall structure for the flushcache function and calculations
 * used in the function were developed based on portions of: 
 * "See Mips Run, 2nd Edition" by Dominic Sweetman, published by Elsevier
 *
 * $Id: flushcache.S 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */
	
#include <mips.h>
	
.text
	.align	4
	.globl flushcache

/**
 * Intialize and flush the L1 instruction and data caches.
 */
	.ent flushcache
flushcache:

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
	.end flushcache
