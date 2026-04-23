/**
 * @file create.c
 */
/* Embedded Xinu, Copyright (C) 2007, 2013.  All rights reserved. */

#include <platform.h>
#include <string.h>
#include <thread.h>

static int thrnew(void);

/**
 * @ingroup threads
 *
 * Create a thread to start running a procedure.
 *
 * @param procaddr
 *      procedure address
 * @param ssize
 *      stack size in bytes
 * @param priority
 *      thread priority (0 is lowest priority)
 * @param name
 *      name of the thread, used for debugging
 * @param nargs
 *      number of arguments that follow
 * @param ...
 *      arguments to pass to thread procedure
 * @return
 *      the new thread's thread id, or ::SYSERR if a new thread could not be
 *      created (not enough memory or thread entries).
 */
tid_typ create(void *procaddr, uint ssize, int priority,
               const char *name, int nargs, ...)
{
    irqmask im;                 /* saved interrupt state               */
    ulong *saddr;               /* stack address                       */
    tid_typ tid;                /* new thread ID                       */
    struct thrent *thrptr;      /* pointer to new thread control block */
    va_list ap;                 /* list of thread arguments            */

    im = disable();

    if (ssize < MINSTK)
    {
        ssize = MINSTK;
    }

    /* Allocate new stack.  */
    saddr = stkget(ssize);
    if (SYSERR == (intptr_t)saddr)
    {
        restore(im);
        return SYSERR;
    }

    /* Allocate new thread ID.  */
    tid = thrnew();
    if (SYSERR == (int)tid)
    {
        stkfree(saddr, ssize);
        restore(im);
        return SYSERR;
    }

    /* Set up thread table entry for new thread.  */
    thrcount++;
    thrptr = &thrtab[tid];

    thrptr->state = THRSUSP;
    thrptr->prio = priority;
    thrptr->stkbase = saddr;
    thrptr->stklen = ssize;
    strlcpy(thrptr->name, name, TNMLEN);
    thrptr->parent = gettid();
    thrptr->hasmsg = FALSE;
    thrptr->memlist.next = NULL;
    thrptr->memlist.length = 0;

    /* Set up default file descriptors.  */
    thrptr->fdesc[0] = CONSOLE; /* stdin  is console */
    thrptr->fdesc[1] = CONSOLE; /* stdout is console */
    thrptr->fdesc[2] = CONSOLE; /* stderr is console */

    /* Set up new thread's stack with context record and arguments.
     * Architecture-specific.  */
    va_start(ap, nargs);
    thrptr->stkptr = setupStack(saddr, procaddr, INITRET, nargs, ap);
    va_end(ap);

    /* Restore interrupts and return new thread TID.  */
    restore(im);
    return tid;
}

/*
 * Obtain a new (free) thread ID.  Returns a free thread ID, or SYSERR if all
 * thread IDs are already in use.  This assumes IRQs have been disabled so that
 * the contents of the threads table are stable.
 */
static int thrnew(void)
{
    int tid;
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
