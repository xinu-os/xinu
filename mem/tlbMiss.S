/**
 * @file     tlb.S
 * @provides tlbMiss
 * TLB miss handler, if this code can remain under 32 instructions we're
 * able to install it at the true TLB exception entry vector (0x80000000).
 * If it grows too large, we'll just put it with the rest and have a simple
 * jump instruction at the entry vector.
 *
 * $Id: tlbMiss.S 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <mips.h>
#include <tlb.h>

.text
   .align   4
   .globl   tlbMiss
   .globl   tlbMissLong

   .set noreorder
   .set noat
/**
 * @fn exchandler tlbMiss(void)
 *
 * TLB miss handler.  Nothing special.  User faults will jump to 0x80000000
 * and execute this code.  Kernel faults of faults within this handler will
 * go directly to tlbMissLong.
 */
   .ent tlbMiss
tlbMiss:
    .set noreorder
    .set noat
    j       tlbMissLong
    nop
    .set at
    .set reorder
    .end tlbMiss

/**
 * fn exchandler tlbMissLong(void)
 *
 * TLB miss handler.  Save the current context of the thread, load up the
 * faulting virtual address, call the handler, load up the page table
 * entries, and return from exception.
 */
    .ent tlbMissLong
tlbMissLong:
    .set noreorder
    .set noat
	addiu   sp, sp, -TLBREC_SIZE
	sw      ra, TLBREC_RA(sp)
	sw      AT, TLBREC_AT(sp)
    .set at
    .set reorder

    /* save current context */
	sw      v0, TLBREC_V0(sp)
	sw      v1, TLBREC_V1(sp)
	sw      a0, TLBREC_A0(sp)
	sw      a1, TLBREC_A1(sp)
	sw      a2, TLBREC_A2(sp)
	sw      a3, TLBREC_A3(sp)
	sw      s0, TLBREC_S0(sp)
	sw      s1, TLBREC_S1(sp)
	sw      s2, TLBREC_S2(sp)
	sw      s3, TLBREC_S3(sp)
	sw      s4, TLBREC_S4(sp)
	sw      s5, TLBREC_S5(sp)
	sw      s6, TLBREC_S6(sp)
	sw      s7, TLBREC_S7(sp)
	sw      s8, TLBREC_S8(sp)
	sw      s9, TLBREC_S9(sp)

    /* handle the miss exception */
	mfc0    a0, CP0_BADVADDR
    la      a1, TLBREC_PTE(sp)
	nop
	jal     tlbMissHandler
	nop

    /* populate the TLB */
    lw      v0, 0(a1)
    lw      v1, 4(a1)
    mtc0    v0, CP0_ENTRYLO0
    mtc0    v1, CP0_ENTRYLO1
    nop
    tlbwr

    /* load original context */
    lw      s9, TLBREC_S9(sp)
    lw      s8, TLBREC_S8(sp)
    lw      s7, TLBREC_S7(sp)
    lw      s6, TLBREC_S6(sp)
    lw      s5, TLBREC_S5(sp)
    lw      s4, TLBREC_S4(sp)
    lw      s3, TLBREC_S3(sp)
    lw      s2, TLBREC_S2(sp)
    lw      s1, TLBREC_S1(sp)
    lw      s0, TLBREC_S0(sp)
    lw      a3, TLBREC_A3(sp)
    lw      a2, TLBREC_A2(sp)
    lw      a1, TLBREC_A1(sp)
    lw      a0, TLBREC_A0(sp)
    lw      v1, TLBREC_V1(sp)
    lw      v0, TLBREC_V0(sp)
    lw      ra, TLBREC_RA(sp)
	addiu   sp, sp, TLBREC_SIZE
	eret
    .end tlbMissLong
