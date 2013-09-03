/**
 * @file     intutils.S
 * Functions to control disabling and enabling of interrupts.
 *
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#include <mips.h>

.text
	.globl enable
	.globl disable
	.globl restore
	.globl enable_cpuirq
	.globl disable_cpuirq
	.globl exlreset
	.globl exlset
	.globl fetchCP0
		
/**
 * @fn void enable(void)
 * Enable all interrupts.
 */
	.ent enable
enable:
	.set noreorder
	mfc0	a1, CP0_STATUS
	/* IE = 1: Master enable bit for SW and HW  */
	li		a2, STATUS_IE  
	or		v0, a1, a2
	jr		ra
	mtc0	v0, CP0_STATUS
	.set reorder
	.end enable

/**
 * @fn irqmask disable(void)
 * Disable interrupts, return old state.
 * @returns state of interrupts before they were disabled
 */
	.ent disable
disable:	
	.set noreorder
	mfc0	v0, CP0_STATUS
 	li		a0, ~STATUS_IE 
	and		a1, v0, a0
	mtc0	a1, CP0_STATUS
	jr		ra
	nop
	.set reorder
	.end disable

/**
 * @fn irqmask restore(irqmask im)
 * Restore interrupts to state in im.
 * @param im irqmask of interrupt state to restore
 * @return state of interrupts when called
 */
	.ent restore
restore:
	.set noreorder
	mfc0	a1, CP0_STATUS
	nop
	or		v0, a1, a0
	jr		ra
	mtc0	v0, CP0_STATUS
	.set reorder
	.end restore

	
/**
 * @fn void enable_cpuirq(uchar irq)
 * Mask on interrupt request source.
 * @param irq index of the interrupt to enable
 */
	.ent enable_cpuirq
enable_cpuirq:
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
	.end enable_cpuirq
		
/**
 * @fn void disable_cpuirq(uchar irq)
 * Mask on interrupt request source.
 * @param irq index of the interrupt to disable
 */
	.ent disable_cpuirq
disable_cpuirq:
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
	.end disable_cpuirq

/**
 * @fn void exlreset(void) 
 * Turn EXL bit off, allowing exceptions/interrupts to occur again.
 */
	.ent exlreset
exlreset:
	.set noreorder
	mfc0	a1, CP0_STATUS
	li		a2, ~STATUS_EXL
	and		v0, a1, a2
	jr		ra
	mtc0	v0, CP0_STATUS
	.set reorder
	.end exlreset

/**
 * @fn void exlset(void) 
 * Turn EXL bit on, preventing exceptions/interrupts.
 */
	.ent exlset
exlset:
	.set noreorder
	mfc0	a1, CP0_STATUS
	li		a2, STATUS_EXL
	or		v0, a1, a2
	jr		ra
	mtc0	v0, CP0_STATUS
	.set reorder
	.end exlset

	.ent fetchCP0
fetchCP0:
	mfc0	v0, CP0_CAUSE
	jr		ra
	.end fetchCP0
