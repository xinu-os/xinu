/**
 * @file     pause.S
 * Platform-dependent code for idling the processor
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

.text
	.align 4	
	.globl pause

/**
 * @fn void pause(void)
 * 
 * Enter some kind of powerdown state (if it exists) that suspends
 * execution until an interrupt is detected.
 */
pause:
	call enable
	hlt
	ret

