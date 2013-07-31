/**
 * @file start.S
 * @provides _start.
 *
 * $Id: start.S 1723 2009-02-28 00:29:03Z mschul $
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

.text
	.align	4
	.globl  _start
	.extern startup
	.extern _end

_start:
	/* disable any interrupts from coming in */
	cli

	movl	$_end,    %eax  # set stack pointer to 64k after end of image
	addl	$64*1024, %eax
	decl	%eax            # 16-byte align stack pointer
	shrl	$0x04, %eax
	shll	$0x04, %eax
	movl	%eax,     %esp  # set stack pointer

	jmp		startup
