/**
 * \file     pause.S
 * Platform-dependent code for idling the processor
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

.text
	.globl pause

	.func pause
pause:
	wfi               /* ARM "wait for interrupt" instruction. */
	mov pc, lr
	.endfunc
