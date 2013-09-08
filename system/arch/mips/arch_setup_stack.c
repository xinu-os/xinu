/**
 * @file arch_setup_stack.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <platform.h>
#include <mips.h>

/** Set up the context record and arguments on the stack for a new thread
 * (MIPS version)  */
void *arch_setup_stack(void *stackaddr, void *procaddr,
                       void *retaddr, uint nargs, va_list ap)
{
    uint i;
    uint nargs_space;
    ulong *saddr = stackaddr;

    /* In the default calling MIPS convention used by gcc, registers $a0 through
     * $a3 are used to pass arguments and any additional arguments spell into
     * the stack.  We do not, however, consider $a0 through $a3 to be part of
     * the context record itself.  Instead, they (along with any additional
     * arguments) are placed after the after the context record.  ctxsw() still
     * loads $a0 through $a3 unconditionally.  Normally this loads garbage, but
     * when switching to a new thread this action will load up to the first four
     * arguments of the new thread procedure.
     *
     * What this means here is that we always leave space for $a0 through $a3.
     *
     * Also note that the MIPS stack should be 8-byte aligned, which in some
     * cases requires that an additional padding word be inserted.
     */

    nargs_space = nargs;
    if (nargs_space < 4)
    {
        nargs_space = 4;
    }
    else if (nargs_space % 2 != CONTEXT_WORDS % 2)
    {
        nargs_space++;
    }

    saddr -= nargs_space;

    for (i = 0; i < nargs; i++)
    {
        saddr[i] = va_arg(ap, ulong);
    }
    for (; i < nargs_space; i++)
    {
        saddr[i] = 0;
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
