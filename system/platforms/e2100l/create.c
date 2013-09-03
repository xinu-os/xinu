/**
 * @file create.c
 * @brief Creates a thread to start running a procedure
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>
#include <stdarg.h>
#include <mips.h>
#include <debug.h>
#include <memory.h>
#include <string.h>

static int thrnew(void);

/* create() implementation for e2100l platform.  See thread.h for generic
 * documentation.  */
tid_typ create(void *procaddr, uint ssize, int priority,
               const char *name, int nargs, ...)
{
    register struct thrent *thrptr;     /* thread control block  */
    ulong *saddr;               /* stack address                      */
    ulong *savargs;             /* pointer to arg saving region       */
    tid_typ tid;                /* stores new thread id               */
    va_list ap;                 /* points to list of var args         */
    int pads;                   /* padding entries in record.         */
    ulong i;
    void INITRET(void);
    irqmask im;

    im = disable();
    if (ssize < MINSTK)
    {
        ssize = MINSTK;
    }
    saddr = stkget(ssize);      /* allocate new stack   */
    tid = thrnew();             /* allocate new thread ID */

    if (((SYSERR) == (int)saddr) || (SYSERR == tid))
    {
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
    *--saddr = (ulong)thrptr->stkbase - thrptr->stklen + sizeof(int);

    if (0 == nargs)
    {
        pads = 4;
    }
    else if (0 == (nargs % 4))  /* pad size to multiple of 4 */
    {
        pads = 0;
    }
    else
    {
        pads = 4 - (nargs % 4);
    }

    for (i = 0; i < pads; i++)
    {
        *--saddr = 0;
    }
    /* reserve space for all args.  */
    for (i = nargs; i > 0; i--)
    {
        *--saddr = 0;
    }
    savargs = saddr;

    /* build the context record as expected by ctxsw               */
    for (i = CONTEXT_WORDS; i > 0; i--)
    {
        *--saddr = 0;
    }
    thrptr->stkptr = saddr;
    /* address of thread entry point  */
    saddr[CONTEXT_WORDS - 1] = (ulong)procaddr;
    /* return address value         */
    saddr[CONTEXT_WORDS - 2] = (ulong)INITRET;

    /* place arguments into activation record */
    va_start(ap, nargs);
    for (i = 0; i < nargs; i++)
    {
        savargs[i] = va_arg(ap, ulong);
    }
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
