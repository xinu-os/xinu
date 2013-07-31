/**
 * @file start.S
 * @provides _start.
 * This is where the firmware environment begins to execute code.
 *
 * $Id: start.S 2372 2011-03-06 02:33:57Z mziwisky $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <mips.h>

.text
	.align	4
	.globl _start
	
/**
 * The firmware will load the image into temporary
 * memory via TFTP and will then jump to this (_start) label.
 * The boot loader (_start) flushes L1 instruction and data caches.
 */
	.ent _start
_start:
	/* Clear and invalidate the L1 instruction and data caches    */
	jal   flushcache
	
	/* Pass control to Xinu kernel.                               */
	j     _startup
	.end _start
