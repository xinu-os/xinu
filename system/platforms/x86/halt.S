/**
 * @file halt.s
 * @provides halt
 *
 * $Id: halt.S 2361 2011-02-22 20:13:36Z mziwisky $
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
	jmp halt
