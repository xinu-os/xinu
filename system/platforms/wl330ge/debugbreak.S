/**
 * @file debugbreak.S
 * Give access to the assembly debug breakpoint command that drops 
 * straight into debug mode.
 */
	
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <mips.h>
	
.text
.align 4	
.set mips32

/**
 * @fn debugbreak
 * 
 * Inserts breakpoint and drops into debug mode.
 * Stores return address to DEPC register and jumps
 * back to it if processor returns from breakpoint.
 */
.globl debugbreak	
debugbreak:
	.set noreorder
	sdbbp
	nop
	jr 	ra
	nop
	.set reorder
