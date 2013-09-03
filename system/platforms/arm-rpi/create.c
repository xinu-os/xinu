/**
 * @file create.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <arm.h>
#include <debug.h>
#include <memory.h>
#include <stdarg.h>
#include <string.h>
#include <thread.h>

static int thrnew(void);

/* create() implementation for arm-rpi platform.  See thread.h for generic
 * documentation.  */
tid_typ create(void *procaddr, uint ssize, int priority,
               const char *name, int nargs, ...)
{

    register struct thrent *thrptr;     /* thread control block  */
    ulong *saddr;               /* stack address                      */
    tid_typ tid;                /* stores new thread id               */
    va_list ap;                 /* points to list of var args         */
    ulong i;
    ulong spilled_nargs;
    ulong *spilled_args;
    ulong reg_nargs;
    void INITRET(void);
    irqmask im;

    im = disable();

    if (ssize < MINSTK)
    {
        ssize = MINSTK;
    }

    saddr = stkget(ssize);      /* allocate new stack   */
    if (SYSERR == (int)saddr)
    {
        restore(im);
        return SYSERR;
    }

    tid = thrnew();             /* allocate new thread ID */
    if (SYSERR == (int)tid)
    {
        stkfree(saddr, ssize);
        restore(im);
        return SYSERR;
    }

    thrcount++;
    thrptr = &thrtab[tid];

    /* setup thread control block for new thread    */
    thrptr->state = THRSUSP;
    thrptr->prio = priority;
    thrptr->stkbase = saddr;
    thrptr->stklen = ssize;
    thrptr->stkptr = saddr;
    strlcpy(thrptr->name, name, TNMLEN);
    thrptr->parent = gettid();
    thrptr->hasmsg = FALSE;
    thrptr->memlist.next = NULL;
    thrptr->memlist.length = 0;


    /* set up default file descriptors */
    thrptr->fdesc[0] = CONSOLE; /* stdin  is console */
    thrptr->fdesc[1] = CONSOLE; /* stdout is console */
    thrptr->fdesc[2] = CONSOLE; /* stderr is console */

    /* Initialize stack with accounting block. */
    *saddr = STACKMAGIC;
    *--saddr = tid;
    *--saddr = thrptr->stklen;
    *--saddr = (ulong)thrptr->stkbase - thrptr->stklen + sizeof(ulong); /* max stack addr */

    /* Reserve space for arguments that spill onto the stack.  */
    if (nargs > 4) {
        spilled_nargs = nargs - 4;
        saddr -= spilled_nargs;
        spilled_args = saddr;
    } else {
        spilled_nargs = 0;
        spilled_args = NULL;
    }
    reg_nargs = nargs - spilled_nargs;

    /* Construct a context record on the stack.  */

    saddr -= CONTEXT_WORDS;
    /* Program counter */
    saddr[CONTEXT_WORDS - 1] = (ulong)procaddr;

    /* Return address */
    saddr[CONTEXT_WORDS - 2] = (ulong)INITRET;

    /* Control bits of program status register
     * (SYS mode, IRQs initially enabled) */
    saddr[CONTEXT_WORDS - 3] = ARM_MODE_SYS | ARM_F_BIT;

    va_start(ap, nargs);

    /* Arguments passed in registers */
    for (i = 0; i < reg_nargs; i++)
    {
        saddr[i] = va_arg(ap, ulong);
    }

    /* Arguments spilled onto stack */
    for (i = 0; i < spilled_nargs; i++)
    {
        spilled_args[i] = va_arg(ap, ulong);
    }

    thrptr->stkptr = saddr;
    va_end(ap);
    restore(im);
    return tid;
}

/**
 * Obtain a new (free) thread id.
 * @return a free thread id, SYSERR if all ids are used
 */
static int thrnew(void)
{
    int tid;                    /* thread id to return     */
    static int nexttid = 0;

    /* check all NTHREAD slots    */
    for (tid = 0; tid < NTHREAD; tid++)
    {
        nexttid = (nexttid + 1) % NTHREAD;
        if (THRFREE == thrtab[nexttid].state)
        {
            return nexttid;
        }
    }
    return SYSERR;
}
