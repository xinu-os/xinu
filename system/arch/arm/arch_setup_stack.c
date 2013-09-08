/**
 * @file arch_setup_stack.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <platform.h>
#include <arm.h>

/* Length of ARM context record in words (includes r0-r11, cpsr, lr, pc).  */
#define CONTEXT_WORDS 15

/* The standard ARM calling convention passes first four arguments in r0-r3; the
 * rest spill onto the stack.  */
#define MAX_REG_ARGS 4

/** Set up the context record and arguments on the stack for a new thread
 * (ARM version)  */
void *arch_setup_stack(void *stackaddr, void *procaddr,
                       void *retaddr, uint nargs, va_list ap)
{
    uint spilled_nargs;
    uint reg_nargs;
    uint i;
    ulong *saddr = stackaddr;

    /* Determine if any arguments will spill onto the stack (outside the context
     * record).  If so, reserve space for them.  */
    if (nargs > MAX_REG_ARGS) {
        spilled_nargs = nargs - MAX_REG_ARGS;
        reg_nargs = MAX_REG_ARGS;
        saddr -= spilled_nargs;
    } else {
        spilled_nargs = 0;
        reg_nargs = nargs;
    }

    /* Possibly skip a word to ensure the stack is aligned on 8-byte boundary.
     */
    if ((CONTEXT_WORDS + spilled_nargs) % 2 != 0)
    {
        *--saddr = 0;
    }

    /* Construct the context record for the new thread.  */

    saddr -= CONTEXT_WORDS;

    /* Arguments passed in registers (part of context record)  */
    for (i = 0; i < reg_nargs; i++)
    {
        saddr[i] = va_arg(ap, ulong);
    }

    for (; i < CONTEXT_WORDS - 3; i++)
    {
        saddr[i] = 0;
    }

    /* Control bits of program status register
     * (SYS mode, IRQs initially enabled) */
    saddr[CONTEXT_WORDS - 3] = ARM_MODE_SYS | ARM_F_BIT;

    /* return address  */
    saddr[CONTEXT_WORDS - 2] = (ulong)retaddr;

    /* program counter  */
    saddr[CONTEXT_WORDS - 1] = (ulong)procaddr;

    /* Arguments spilled onto stack (not part of context record)  */
    for (i = 0; i < spilled_nargs; i++)
    {
        saddr[CONTEXT_WORDS + i] = va_arg(ap, ulong);
    }

    /* Return "top" of stack (lowest address).  */
    return saddr;
}
