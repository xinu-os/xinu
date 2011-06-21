/**
 * @file     pause.S
 * @provides pause.
 * Platform-dependent code for idling the processor
 *
 * $Id: pause.S 2086 2009-10-06 22:24:27Z brylow $
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

