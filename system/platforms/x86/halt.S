/**
 * @file halt.s
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

.text
	.align 4
	.globl	halt

/**
 * @fn void halt(void)
 *
 * Essentially this is a busy wait, however the opcode 'hlt' puts the
 * processor in a powersave state (which can be awoken for arbitrary
 * reasons)
 */
halt:
	hlt
	jmp halt
