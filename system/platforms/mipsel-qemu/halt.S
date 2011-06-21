/**
 * @file halt.s
 * @provides halt.
 *
 * $Id: halt.S 2086 2009-10-06 22:24:27Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <mips.h>
	
.text
	.align 4
	.globl	halt

/**
 * @fn void halt(void)
 *
 * Essentially this is a busy wait, however the opcode 'wait' puts the
 * processor in a powersave state (which can be awoken for arbitrary
 * reasons)
 */
	.ent halt
halt:
	wait
	j	halt
	.end halt

