/**
 * @file     pause.S
 * Platform-dependent code for idling the processor
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <mips.h>

.text
	.align 4	
	.globl pause

/**
 * @fn void pause(void)
 * 
 * Enter some kind of powerdown state (if it exists) that suspends
 * execution until an interrupt is detected.
 */
	.ent pause
pause:
	wait
	jr	ra
	.end pause

