/**
 * @file     clkupdate.S
 * @provides clkupdate.
 * Platform-dependent code for clock interrupt maintenance.
 *
 * $Id: clkupdate.S 2086 2009-10-06 22:24:27Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <mips.h>

.text
	.align 4
	.globl clkupdate
	.globl clkcount

/**
 * @fn void clkupdate(ulong cycles)
 * 
 * Normal case: COMPARE is increased by cycles and stored.
 * Abnormal case: We've lost so much time COUNT > COMPARE+cycles.
 * Solution: Set COMPARE to COUNT+cycles
 */
clkupdate:
	.set noreorder
	mfc0 v0, CP0_COMPARE     /* v0 = COMPARE                      */
	mfc0 v1, CP0_COUNT       /* v1 = COUNT                        */
	addu v0, v0, a0          /* v0 = COMPARE + cycles             */
	bleu v0, v1, compare_up  /* v0 <= COUNT, then goto compare_up */
	nop
	jr	 ra 
	mtc0 v0, CP0_COMPARE     /* Update COMPARE                    */
	.set reorder

/* we've missed too many cycles, update COMPARE                   */
compare_up:
	.set noreorder
	addu a0, v1, a0         /* a0 = COUNT + cycles                */
	jr   ra
	mtc0 a0, CP0_COMPARE    /* COMPARE = a0                       */
	.set reorder

/**
  * @fn void clkcount(void)
  * Return free-running clock count.
  */
clkcount:
	mfc0 v0, CP0_COUNT
	jr   ra

