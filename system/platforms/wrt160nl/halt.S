/**
 * @file halt.s
 * @provides halt.
 *
 * $Id: halt.S 2092 2009-10-07 23:08:36Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

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

