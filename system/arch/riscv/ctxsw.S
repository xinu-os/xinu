/**
 * @file ctxsw.S
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <riscv.h>

.globl ctxsw

/*------------------------------------------------------------------------
 *  ctxsw  -  Switch from one thread context to another.
 *------------------------------------------------------------------------
 *
 * This is the RISCV version.  How it works: we have to save s0-s11
 * ra, and sp since s0-s11 are saved registers, ra  needs to be loaded into the
 * pc when this context is switched to again. Registers a0-a7 are
 * caller-save so they do not need not be saved, but they are pushed anyway
 * since they are part of the context constructed by create() to pass thread arguments.
 *
 * We do not need to do anything about the CSRs here, since:
 *
 *   - We do all our context switches to/from the same mode (namely, Machine mode).
 *   - The RISC-V ABI does not use CSRs
 *   - resched() takes care of saving/restoring whether interrupts are enabled
 *         or not when resuming a thread that has been switched out.
 *   - Xinu never makes changes to the CSRs not already covered above
 *
 * However, interrupts are disabled when ctxsw() is called from resched(), but
 * we want interrupts to be enabled when starting a *new* thread, which
 * resched() does not take care of.  We solve this by including the machine status
 * register value into the context and restoring it when we see that we're
 * switching to the newly created thread
 */

ctxsw:
	.func ctxsw
	// reserve space
	addi sp, sp, -REGBYTES*23
	// save registers
	sd a7, REGBYTES*22(sp)	
	sd a6, REGBYTES*21(sp)	
	sd a5, REGBYTES*20(sp)
	sd a4, REGBYTES*19(sp)
	sd a3, REGBYTES*18(sp)
	sd a2, REGBYTES*17(sp)
	sd a1, REGBYTES*16(sp)
	sd a0, REGBYTES*15(sp)
	sd s11, REGBYTES*14(sp)
	sd s10, REGBYTES*13(sp)
	sd s9, REGBYTES*12(sp)
	sd s8, REGBYTES*11(sp)
	sd s7, REGBYTES*10(sp)	
	sd s6, REGBYTES*9(sp)
	sd s5, REGBYTES*8(sp)
	sd s4, REGBYTES*7(sp)
	sd s3, REGBYTES*6(sp)
	sd s2, REGBYTES*5(sp)
	sd s1, REGBYTES*4(sp)
	sd s0, REGBYTES*3(sp)
	// save mstatus
	csrr t2, mstatus
	sd t2, REGBYTES*2(sp)
	// we jump at end, jump address is different
	// from return address only when switching to newly
	// created context
	sd ra,REGBYTES*1(sp)
	sd ra,(sp)
	//save modified sp at oldthr->stkptr
	sd sp, (a0)
	// get saved sp from newthrd->stkptr
	ld sp, (a1)

	// restore t0 <- saved jump address 
	ld t0, (sp)
	ld ra, REGBYTES*1(sp)
	// t2 <- saved mstatus
	ld t2, REGBYTES*2(sp)

	// restore saved registers
	ld s0, REGBYTES*3(sp)
	ld s1, REGBYTES*4(sp)
	ld s2, REGBYTES*5(sp)
	ld s3, REGBYTES*6(sp)
	ld s4, REGBYTES*7(sp)
	ld s5, REGBYTES*8(sp)
	ld s6, REGBYTES*9(sp)
	ld s7, REGBYTES*10(sp)	
	ld s8, REGBYTES*11(sp)
	ld s9, REGBYTES*12(sp)
	ld s10, REGBYTES*13(sp)
	ld s11, REGBYTES*14(sp)

	ld a0, REGBYTES*15(sp)
	ld a1, REGBYTES*16(sp)
	ld a2, REGBYTES*17(sp)
	ld a3, REGBYTES*18(sp)
	ld a4, REGBYTES*19(sp)
	ld a5, REGBYTES*20(sp)
	ld a6, REGBYTES*21(sp)	
	ld a7, REGBYTES*22(sp)	

	// restore sp
	addi sp, sp, REGBYTES*23

	// check if interrupts are disabled in saved mstatus
	andi t3, t2, MSTATUS_MIE
	beq t3,x0, end_ctxsw
	// interrupts enabled (new thread)
	// now tricky part: we restore mstatus as it was saved
	// in the context _except_ the interrupt enable bit:
	// t2 <- t2 & ~MSTATUS_MIE
	andi t2, t2, -9
	csrrw t2,  mstatus, t2
	// load jump address to mepc
	csrrw t1, mepc, t0
	// mret will enable interrupts and
	// jump to the newly created thread 
	mret
end_ctxsw:
	// interrupts disabled: return to old thread
	// pc is restored, next address is put into
	// t1 and ignored
	jalr t1,t0,0
	.endfunc
