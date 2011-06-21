/**
 * @file     ctxsw.s
 * @provides ctxsw.
 *
 * $Id: ctxsw.S 2086 2009-10-06 22:24:27Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <mips.h>
	
.text
	.align 4
	.globl	ctxsw

/*------------------------------------------------------------------------
 *  ctxsw  -  Switch from one thread context to another.
 *------------------------------------------------------------------------
 */
	.ent ctxsw
ctxsw:
	/* build context record on stack                               */
	addiu	sp, sp, -CONTEXT
	sw	ra, CONTEXT-4(sp)
	sw	ra, CONTEXT-8(sp)
	/* save callee-save ("non-volatile") registers                 */
	sw	s0, S0_CON(sp)
	sw	s1, S1_CON(sp)
	sw	s2, S2_CON(sp)
	sw	s3, S3_CON(sp)
	sw	s4, S4_CON(sp)
	sw	s5, S5_CON(sp)
	sw	s6, S6_CON(sp)
	sw	s7, S7_CON(sp)
	sw	s8, S8_CON(sp)
	sw	s9, S9_CON(sp)
	/* save outgoing stack pointer                                 */
	sw	sp, 0(a0)
	/* load incoming stack pointer                                 */
	lw	sp, 0(a1)
	/* restore callee-save ("non-volatile") registers              */
	lw	s0, S0_CON(sp)
	lw	s1, S1_CON(sp)
	lw	s2, S2_CON(sp)
	lw	s3, S3_CON(sp)
	lw	s4, S4_CON(sp)
	lw	s5, S5_CON(sp)
	lw	s6, S6_CON(sp)
	lw	s7, S7_CON(sp)
	lw	s8, S8_CON(sp)
	lw	s9, S9_CON(sp)
	/* restore argument registers                                  */
	lw	a0, CONTEXT(sp)	
	lw	a1, CONTEXT+4(sp)	
	lw	a2, CONTEXT+8(sp)	
	lw	a3, CONTEXT+12(sp)	
	/* tear down context record and return                         */
	lw	v0, CONTEXT-4(sp)
	lw	ra, CONTEXT-8(sp)
	addiu	sp, sp, CONTEXT
	jr	v0
	.end ctxsw

