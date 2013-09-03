/**
 * @file intutils.S
 *
 * Functions to enable, disable, or restore global interrupts on the ARM.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

.globl enable
.globl disable
.globl restore

/**
 * @fn void enable(void)
 *
 * Enable interrupts globally.
 */
enable:
	.func enable
	cpsie i
	mov pc, lr
	.endfunc

/**
 * @fn irqmask disable(void)
 *
 * Disable interrupts globally and returns the old state.
 * @return state of interrupts before they were disabled
 */
disable:
	.func disable
	mrs r0, cpsr
	cpsid i
	mov pc, lr
	.endfunc

/**
 * @fn irqmask restore(irqmask)
 *
 * Restores the global interrupt mask to a previous state.
 * @param im
 *     irqmask of interrupt state to restore
 * @return state of interrupts when called
 */
restore:
	.func restore
	msr cpsr_c, r0
	mov pc, lr
	.endfunc
