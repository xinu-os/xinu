/**
 * @file intutils.S
 *
 * Functions to enable, disable, or restore global interrupts on the RISC-V
 *
 */
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */
#include <riscv.h>

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
	csrrsi a0, mstatus, MSTATUS_MIE
	ret
	.endfunc

/**
 * @fn irqmask disable(void)
 *
 * Disable interrupts globally and returns the old state.
 * @return state of interrupts before they were disabled:
 *      0x0 - was enabled,
 *	0x8 - if it was disabled
 *	      (i.e. the inverse of the MIE bit)
 *
 */
disable:
	.func disable
	csrrci a0, mstatus, MSTATUS_MIE
	not a0,a0
	andi a0,a0,MSTATUS_MIE
        ret
	.endfunc

/**
 * @fn irqmask restore(irqmask)
 *
 * Restores the global interrupt mask to a previous state.
 * @param im
 *     irqmask of interrupt state to restore:
 *     0 - enable, not 0 - disable
 * @return state of interrupts when called
 */
restore:
	.func restore
	// branch if is enable
	beq a0,x0, restore_set_mie
	csrrci a0, mstatus, MSTATUS_MIE
	j restore_return
restore_set_mie:
	csrrsi a0, mstatus, MSTATUS_MIE
restore_return:
	not  a0, a0
	andi a0, a0, MSTATUS_MIE
	ret
	.endfunc


