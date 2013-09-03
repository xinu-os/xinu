/**
 * @file create.c
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */
  
#include <stddef.h>
#include <thread.h>
#include <stdarg.h>
#include <memory.h>
#include <string.h>

static int thrnew(void);

/* create() implementation for x86 platform.  See thread.h for generic
 * documentation.  */
tid_typ create(void *procaddr, uint ssize, int priority,
	       const char *name, int nargs, ...)
{
    register struct thrent *thrptr;    /* thread control block    */
    ulong   *saddr;         /* stack address                   */
    tid_typ tid;            /* stores new thread id            */
    ulong   savsp;          /* for remembering stack pointer   */
    ulong   *ap;            /* points to list of var args      */
    void    INITRET(void);
    irqmask im;

    im = disable();
    if (ssize < MINSTK)
    {
        ssize = MINSTK;
    }
    saddr = stkget(ssize);     /* allocate new stack     */
    tid   = thrnew();          /* allocate new thread ID */
                                     
    if ((SYSERR == (int)saddr) || (SYSERR == tid))
    {
        restore(im);
        return SYSERR;
    }

    thrcount++;
    thrptr = &thrtab[tid];
    
    /* setup thread control block for new thread    */    
    thrptr->state   = THRSUSP;
    thrptr->prio    = priority;
    thrptr->stkbase = saddr;
    thrptr->stklen  = ssize;
    thrptr->stkptr  = saddr;
    strlcpy(thrptr->name, name, TNMLEN);
    thrptr->parent  = gettid();
    thrptr->hasmsg = FALSE;
    thrptr->memlist.next = NULL;
    thrptr->memlist.length = 0;

    /* set up default file descriptors */
    thrptr->fdesc[0] = CONSOLE; /* stdin  is console */
    thrptr->fdesc[1] = CONSOLE; /* stdout is console */
    thrptr->fdesc[2] = CONSOLE; /* stderr is console */

    /* Initialize stack with accounting block. */
    *saddr   = STACKMAGIC;
    *--saddr = tid;
    *--saddr = thrptr->stklen;
    *--saddr = (ulong)thrptr->stkbase - thrptr->stklen + sizeof(int);
    savsp    = (ulong)saddr;
    
    /* push arguments */
    ap = (ulong *)(&nargs + 1) + (nargs - 1);
    while ( nargs > 0 ) 
    { 
        *--saddr =  *ap--;
        nargs--;
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
    thrptr->stkptr = (void *)saddr;

    restore(im);
    return tid;
}

/**
 * Obtain a new (free) thread id.
 * @return a free thread id, SYSERR if all ids are used
 */
static int thrnew(void)
{
    int    tid;            /* thread id to return     */
    static int nexttid = 0;

    /* check all NTHREAD slots    */
    for (tid = 0 ; tid < NTHREAD ; tid++) 
    {
        nexttid = (nexttid + 1) % NTHREAD;
        if (THRFREE == thrtab[nexttid].state)
        {
            return nexttid;
        }
    }
    return SYSERR;
}
