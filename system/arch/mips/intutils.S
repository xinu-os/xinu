/**
 * @file     intutils.S
 * Functions to control disabling and enabling of interrupts.
 *
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#include <mips.h>

.globl enable
.globl disable
.globl restore
.globl enable_cpuirq
.globl disable_cpuirq
.globl exlreset
.globl exlset
		
/**
 * @fn void enable(void)
 * Enable all interrupts.
 */
enable:
	.func enable
	.set noreorder
	mfc0	a1, CP0_STATUS
	/* IE = 1: Master enable bit for SW and HW  */
	li		a2, STATUS_IE  
	or		v0, a1, a2
	jr		ra
	mtc0	v0, CP0_STATUS
	.set reorder
	.endfunc

/**
 * @fn irqmask disable(void)
 * Disable interrupts, return old state.
 * @returns state of interrupts before they were disabled
 */
disable:	
	.func disable
	.set noreorder
	mfc0	v0, CP0_STATUS
 	li		a0, ~STATUS_IE 
	and		a1, v0, a0
	mtc0	a1, CP0_STATUS
	jr		ra
	nop
	.set reorder
	.endfunc

/**
 * @fn irqmask restore(irqmask im)
 * Restore interrupts to state in im.
 * @param im irqmask of interrupt state to restore
 * @return state of interrupts when called
 */
restore:
	.func restore
	.set noreorder
	mfc0	a1, CP0_STATUS
	nop
	or		v0, a1, a0
	jr		ra
	mtc0	v0, CP0_STATUS
	.set reorder
	.endfunc

	
/**
 * @fn void enable_cpuirq(uchar irq)
 * Mask on interrupt request source.
 * @param irq index of the interrupt to enable
 */
enable_cpuirq:
	.func enable_cpuirq
	.set noreorder
	mfc0	a1, CP0_STATUS
	addi	a3, zero, 7
	and		a0, a0, a3
	addi	a0, a0, 8
	li		a2, 1
	sllv	a2, a2, a0
	or		v0, a1, a2
	jr		ra
	mtc0	v0, CP0_STATUS
	.set reorder
	.endfunc
		
/**
 * @fn void disable_cpuirq(uchar irq)
 * Mask on interrupt request source.
 * @param irq index of the interrupt to disable
 */
disable_cpuirq:
	.func disable_cpuirq
	.set noreorder
	mfc0	a1, CP0_STATUS
	addi	a0, a0, 8
	li	a2, 1
	sllv	a2, a2, a0
	nor	a2, a2, zero
	and	v0, a1, a2
	jr	ra
	mtc0	v0, CP0_STATUS
	.set reorder
	.endfunc

/**
 * @fn void exlreset(void) 
 * Turn EXL bit off, allowing exceptions/interrupts to occur again.
 */
exlreset:
	.func exlreset
	.set noreorder
	mfc0	a1, CP0_STATUS
	li		a2, ~STATUS_EXL
	and		v0, a1, a2
	jr		ra
	mtc0	v0, CP0_STATUS
	.set reorder
	.endfunc

/**
 * @fn void exlset(void) 
 * Turn EXL bit on, preventing exceptions/interrupts.
 */
exlset:
	.func exlset
	.set noreorder
	mfc0	a1, CP0_STATUS
	li		a2, STATUS_EXL
	or		v0, a1, a2
	jr		ra
	mtc0	v0, CP0_STATUS
	.set reorder
	.endfunc
