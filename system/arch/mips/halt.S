/**
 * @file halt.S
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <mips.h>
	
.globl	halt

/**
 * @fn void halt(void)
 *
 * Essentially this is a busy wait, however the opcode 'wait' puts the
 * processor in a powersave state (which can be awoken for arbitrary
 * reasons)
 */
halt:
	.func halt
	wait
	j	halt
	.endfunc

