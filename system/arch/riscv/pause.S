/**
 * @file pause.S
 * Platform-dependent code for idling the processor.
 */
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

.globl pause

pause:
	.func pause
	wfi  /* Wait For Interrupt instruction. */
	ret
	.endfunc
