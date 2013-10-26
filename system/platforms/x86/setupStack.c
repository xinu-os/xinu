/**
 * @file setupStack.c
 */
/* Embedded Xinu, Copyright (C) 2007, 2013.  All rights reserved. */

#include <platform.h>
#include <thread.h>

/** Set up the context record and arguments on the stack for a new thread
 * (x86 version)  */
void *setupStack(void *stackaddr, void *procaddr,
                 void *retaddr, uint nargs, va_list ap)
{
    ulong *saddr = stackaddr;
    ulong  savsp;          /* for remembering stack pointer   */
    uint i;

    savsp = (ulong)saddr;

    /* push arguments */
    saddr -= nargs;
    for (i = 0; i < nargs; i++)
    {
        saddr[i] = va_arg(ap, ulong);
    }

    *--saddr     = (ulong)INITRET;
    *--saddr     = (ulong)procaddr;
    *--saddr     = savsp;
    savsp        = (ulong)saddr;

    /* now we must emulate what ctxsw expects: flags, regs, and old SP */
    /* emulate 386 "pushal" instruction */
    *--saddr     = 0;
    *--saddr     = 0; /* %eax */
    *--saddr     = 0; /* %ecx */
    *--saddr     = 0; /* %edx */
    *--saddr     = 0; /* %ebx */
    *--saddr     = 0; /* %esp; "popal" doesn't actually pop it, so 0 is ok */
    *--saddr     = savsp; /* %ebp */
    *--saddr     = 0; /* %esi */
    *--saddr     = 0; /* %edi */
    return saddr;
}
