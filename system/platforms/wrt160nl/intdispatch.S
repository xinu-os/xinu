/**
 * @file     intdispatch.S
 * @provides intdispatch, savestate, restorestate.
 *
 * $Id: intdispatch.S 2106 2009-10-28 21:51:16Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#include <mips.h>
#include <interrupt.h>

.text
	.align 4
	.globl intdispatch

/**
 * @fn void intdispatch(void)
 *
 * Main interrupt dispatcher.
 * Jumps to savestate, which does full context save.  
 * First 32 words of intdispatch are copied to interrupt handler hardware
 *  address, so first code block cannot exceed 32 instructions.
 */
	.ent intdispatch
intdispatch:
	.set noreorder
	.set noat
	mfc0	k0, CP0_CAUSE       /* 0x00: Get interrupt/exception cause */
	addiu	k1, zero, CAUSE_EXC /* 0x01: Load exception bit mask       */
	and	k0, k0, k1          /* 0x02: Check cause against exception */
	beq	k0, zero, 1f        /* 0x03: If not exception, savestate   */
	lui	k1, %hi(TRAPVEC_ADDR) /* 0x04: Load high bits of TRAPVEC   */
	addiu	k1, %lo(TRAPVEC_ADDR) /* 0x05: Load low bits of TRAPVEC    */
	addu	k1, k1, k0          /* 0x06: index into TRAPVEC table      */
	lw	k0, 0(k1)           /* 0x07: load exception vector address */
	nop                         /* 0x08: delay slot                    */
	beq	k0, zero, 1f        /* 0x09: if null vector, savestate     */
	nop                         /* 0x0a: delay slot                    */
	jr	k0                  /* 0x0b: vector exists, go to it.      */
	nop                         /* 0x0c: delay slot                    */
1:	j	savestate           /* 0x0d: jump to long handler          */
	nop                         /* 0x0e: filler                        */
	nop                         /* 0x0f: filler                        */
	nop                         /* 0x10: filler                        */
	nop                         /* 0x11: filler                        */
	nop                         /* 0x12: filler                        */
	nop                         /* 0x13: filler                        */
	nop                         /* 0x14: filler                        */
	nop                         /* 0x15: filler                        */
	nop                         /* 0x16: filler                        */
	nop                         /* 0x17: filler                        */
	nop                         /* 0x18: filler                        */
	nop                         /* 0x19: filler                        */
	nop                         /* 0x1a: filler                        */
	nop                         /* 0x1b: filler                        */
	nop                         /* 0x1c: filler                        */
	nop                         /* 0x1d: filler                        */
	nop                         /* 0x1e: filler                        */
	nop                         /* 0x1f: filler                        */
	.set at
	.set reorder

/**
 * Save processor state and jump to main handler.  The IRQ Handler saves all
 * relevant volatile state on the stack, and transfers control to a C function
 * for decoding the interrupt source and picking the correct vector.
 */
savestate:
	.set noreorder
	.set noat
	addiu	sp, sp, -IRQREC_SIZE

	/* Save	AT (assembler temp) before any synthetic opcodes. */
	sw		AT, IRQREC_AT(sp)
	/* Save interrupt/exception CAUSE value.                  */
	mfc0	k0, CP0_CAUSE
	/* Save program counter from interrupt/exception source.  */
	mfc0	k1, CP0_EPC
	sw		k0, IRQREC_CAUSE(sp)
	/* Save STATUS register from coprocessor.                 */
	mfc0	k0, CP0_STATUS
	sw		k1, IRQREC_EPC(sp)
	sw		k0, IRQREC_STATUS(sp)
	.set at
	.set reorder

	/* Save all general purpose registers.                    */
	sw		v0, IRQREC_V0(sp)
	sw		v1, IRQREC_V1(sp)
	sw		a0, IRQREC_A0(sp)
	sw		a1, IRQREC_A1(sp)
	sw		a2, IRQREC_A2(sp)
	sw		a3, IRQREC_A3(sp)
	sw		t0, IRQREC_T0(sp)
	sw		t1, IRQREC_T1(sp)
	sw		t2, IRQREC_T2(sp)
	sw		t3, IRQREC_T3(sp)
	sw		t4, IRQREC_T4(sp)
	sw		t5, IRQREC_T5(sp)
	sw		t6, IRQREC_T6(sp)
	sw		t7, IRQREC_T7(sp)
	sw		s0, IRQREC_S0(sp)
	sw		s1, IRQREC_S1(sp)
	sw		s2, IRQREC_S2(sp)
	sw		s3, IRQREC_S3(sp)
	sw		s4, IRQREC_S4(sp)
	sw		s5, IRQREC_S5(sp)
	sw		s6, IRQREC_S6(sp)
	sw		s7, IRQREC_S7(sp)
	sw		t8, IRQREC_T8(sp)
	sw		t9, IRQREC_T9(sp)
	sw		k0, IRQREC_K0(sp)
	sw		k1, IRQREC_K1(sp)
	sw		gp, IRQREC_S8(sp)
	sw		sp, IRQREC_SP(sp)
	sw		fp, IRQREC_S9(sp)
	sw		ra, IRQREC_RA(sp)
	sw		zero, IRQREC_ZER(sp)

	/* Save a couple of other volatile regs, hi and lo.       */
	mfhi	t0
	mflo	t1
	sw		t0, IRQREC_HI(sp)
	sw		t1, IRQREC_LO(sp)

	/* Call high-level trap handler.                          */
	lw		a0, IRQREC_CAUSE(sp)
	move	a1, sp
	li		a2, CAUSE_EXC
	and		a2, a0, a2
	beq		a2, zero, 1f
	jal		exception
	b		restorestate
1:
	jal	    dispatch

/**
 * Common return routine for exceptions and interrupts.  All interrupts
 * that save state will eventually return here, where the state is restored
 * and control is returned to the point where the interrupt occurred.
 */
restorestate:
	/* Restore HI and LO special regs.                        */
	lw		t0, IRQREC_HI(sp)
	lw		t1, IRQREC_LO(sp)
	mthi	t0
	mtlo	t1

	/* Restore general purpose regs.                          */
	/* lw  zero, IRQREC_ZER(sp)                               */
	lw		ra, IRQREC_RA(sp)
	lw		fp, IRQREC_S9(sp)
	/* lw  sp, IRQREC_SP(sp)                                  */
 	lw		gp, IRQREC_S8(sp)
	/* lw  k1, IRQREC_K1(sp)                                  */
 	/* lw  k0, IRQREC_K0(sp)                                  */
 	lw		t9, IRQREC_T9(sp)
 	lw		t8, IRQREC_T8(sp)
 	lw		s7, IRQREC_S7(sp)
 	lw		s6, IRQREC_S6(sp)
 	lw		s5, IRQREC_S5(sp)
 	lw		s4, IRQREC_S4(sp)
 	lw		s3, IRQREC_S3(sp)
 	lw		s2, IRQREC_S2(sp)
 	lw		s1, IRQREC_S1(sp)
 	lw		s0, IRQREC_S0(sp)
 	lw		t7, IRQREC_T7(sp)
 	lw		t6, IRQREC_T6(sp)
 	lw		t5, IRQREC_T5(sp)
 	lw		t4, IRQREC_T4(sp)
 	lw		t3, IRQREC_T3(sp)
 	lw		t2, IRQREC_T2(sp)
 	lw		t1, IRQREC_T1(sp)
 	lw		t0, IRQREC_T0(sp)
 	lw		a3, IRQREC_A3(sp)
 	lw		a2, IRQREC_A2(sp)
 	lw		a1, IRQREC_A1(sp)
 	lw		a0, IRQREC_A0(sp)
 	lw		v1, IRQREC_V1(sp)
 	lw		v0, IRQREC_V0(sp)

	.set noreorder
	.set noat
	/* restore EPC                                            */
	lw		k0, IRQREC_EPC(sp)
 	/* restore AT                                             */
 	lw		AT, IRQREC_AT(sp)
	mtc0	k0, CP0_EPC
 	/* restore STATUS                                         */
	lw		k1, IRQREC_STATUS(sp)
	/* restore stack pointer */
	addiu	sp, sp, IRQREC_SIZE
	mtc0	k1, CP0_STATUS
	nop
	eret
	nop
	nop
	.set at
	.set reorder
	.end intdispatch

