/**
 * @file ctxsw.S
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <arm.h>

.globl ctxsw

/*------------------------------------------------------------------------
 *  ctxsw  -  Switch from one thread context to another.
 *------------------------------------------------------------------------
 *
 * This is the ARM version.  How it works: we have to save r4-r11 and lr, since
 * r4-r11 are callee-save and lr needs to be loaded into the pc when this
 * context is switched to again.  Registers r0-r3 are caller-save so they do
 * *not* need not be saved, but they are pushed anyway to leave space since they
 * are part of the context constructed by create() to pass thread arguments.
 *
 * When restoring a context, we pop both the lr and pc.  These are both set to
 * appropriate values in create().  But when saving a context below, we only
 * have an appropriate value for pc--- namely, the lr, a.k.a. the address
 * ctxsw() will return to.  The lr at that instruction is unknown.  However,
 * this is irrelevant because the lr is caller-save, and we can simply push a
 * garbage value from r13 instead.
 *
 * We almost don't need to do anything about the CPSR here, since:
 *
 *   - We do all our context switches to/from the same mode (namely, SYS mode).
 *   - The ARM ABI does not expect comparison flags in the CPSR to be preserved
 *         across function calls.
 *   - resched() takes care of saving/restoring whether interrupts are enabled
 *         or not when resuming a thread that has been switched out.
 *   - Xinu never makes changes to the CPSR not already covered above, such as
 *         switching to executing Thumb instructions.
 *
 * However, interrupts are disabled when ctxsw() is called from resched(), but
 * we want interrupts to be enabled when starting a *new* thread, which
 * resched() does not take care of.  We solve this by including the control bits
 * of the current program status register in the context and adding a line of
 * code to create() that sets the control bits of new threads such that
 * interrupts are enabled.
 *------------------------------------------------------------------------*/
ctxsw:
	.func ctxsw
	mrs r12, cpsr
	push {r0-r14}

	str sp, [r0]
	ldr sp, [r1]

	pop {r0-r12}
	msr cpsr_c, r12
	pop {lr, pc}
	.endfunc
