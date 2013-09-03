/**
 * @file halt.S
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

.text
	.globl halt

#include <arm.h>

/**
 * @fn void halt(void)
 *
 * Halt the system by sending the processor into an infinite loop.
 */
halt:
	.func halt
	msr cpsr_c, #ARM_I_BIT | ARM_F_BIT
_loop:
	wfi
	b _loop
	.endfunc
