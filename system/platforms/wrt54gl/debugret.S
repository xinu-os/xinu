/**
 * @file debugret.s
 * @provides debugret, debugretstart.
 * Gives userret functionality of killing thread with additional 
 * no-op at beginning for GDB
 *
 * $Id: debugret.S 2086 2009-10-06 22:24:27Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

	.section .mdebug.abi32
	.previous
	.text
	.align	2
	.globl	debugret
        .globl	debugretstart

/**
 * @fn debugretstart
 * prefixed no-op for GDB compatibility
 */
debugretstart:
	.ent	debugretstart
	nop
	.end	debugretstart
/**
 * @fn debugret
 * kills thread in same fashion as userret
 */
debugret:
	.ent	debugret
	.frame	$fp,24,$31		# vars= 0, regs= 2/0, args= 16, gp= 0
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	nop
	addiu	$sp,$sp,-24
	sw	$31,20($sp)
	sw	$fp,16($sp)
	move	$fp,$sp
	jal	gettid
	move	$4,$2
	jal	kill
	move	$sp,$fp
	lw	$31,20($sp)
	lw	$fp,16($sp)
	addiu	$sp,$sp,24
	j	$31
	.end	debugret
	.size	debugret, .-debugret
	.ident	"GCC: (GNU) 4.2.0"
