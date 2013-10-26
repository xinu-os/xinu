/**
 * @file setupStack.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <platform.h>
#include <kernel.h>
#include <mips.h>

/** Set up the context record and arguments on the stack for a new thread
 * (MIPS version)  */
void *setupStack(void *stackaddr, void *procaddr,
                 void *retaddr, uint nargs, va_list ap)
{
    uint i;
    ulong *saddr = stackaddr;

    /* In the default calling MIPS convention used by gcc, registers $a0 through
     * $a3 are used to pass arguments and any additional arguments spill into
     * the stack.  However, immediately prior (lower address) to any spilled
     * arguments, the caller must still leave 4 words of space so that callee
     * can temporarily store arguments there if it chooses.
     *
     * Therefore, we do not consider $a0 through $a3 to be part of the context
     * record itself, but they are still loaded by ctxsw().  This will load up
     * to the first four arguments when starting a new thread.
     */

    saddr -= max(nargs, 4);

    /* The MIPS stack should be 8-byte aligned on entry to all procedures.  */
    if ((ulong)saddr & 0x4)
    {
        --saddr;
    }

    /* Save arguments on stack.  */
    for (i = 0; i < nargs; i++)
    {
        saddr[i] = va_arg(ap, ulong);
    }

    /* Construct context record.  */
    saddr -= CONTEXT_WORDS;

    for (i = 0; i < CONTEXT_WORDS - 2; i++)
    {
        saddr[i] = 0;
    }

    /* return address  */
    saddr[CONTEXT_WORDS - 2] = (ulong)retaddr;

    /* program counter  */
    saddr[CONTEXT_WORDS - 1] = (ulong)procaddr;

    return saddr;
}
