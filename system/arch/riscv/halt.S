/**
 * @file halt.S
 */
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

.globl halt

/**
 * @fn void halt(void)
 *
 * Halt the system by sending the processor into an infinite loop.
 */
halt:
	.func halt

_loop:
	/* Wait for interrupt.  */
	wfi
	j _loop
	.endfunc
