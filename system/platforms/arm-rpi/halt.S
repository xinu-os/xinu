/**
 * @file halt.S
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

.globl halt

#include <arm.h>

/**
 * @fn void halt(void)
 *
 * Halt the system by sending the processor into an infinite loop.
 */
halt:
	.func halt

_loop:
	/* Disable IRQs and FIQs.  */
	msr cpsr_c, #ARM_I_BIT | ARM_F_BIT

	/* Wait for interrupt.  */
	wfi

	/* Shouldn't be necessary, but repeat just in case.  */
	b _loop
	.endfunc
