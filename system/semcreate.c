/**
 * @file semcreate.c
 * @provides semcreate, semalloc.
 *
 * $Id: semcreate.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <semaphore.h>
#include <interrupt.h>

static semaphore semalloc(void);

/**
 * Create and initialize a semaphore, returning its ID.
 * @param count  resources available.
 * @return new semaphore ID on success, SYSERR on failure
 */
semaphore semcreate(int count)
{
    register semaphore sem;
    irqmask im;

    im = disable();             /* disable interrupts    */
    sem = semalloc();           /* request new semaphore */
    if (sem != SYSERR && count >= 0)    /* safety check          */
    {
        semtab[sem].count = count;      /* initialize count      */
        restore(im);            /* restore interrupts    */
        return sem;             /* return semaphore id   */
    }

    restore(im);
    return SYSERR;
}

/**
 * Allocate an unused semaphore and return its ID.
 * Scan the global semaphore table for a free entry, mark the entry
 * used, and return the new semaphore
 * @return available semaphore ID on success, SYSERR on failure
 */
static semaphore semalloc(void)
{
    int sem = 0;                /* sempahore to return */
    static int nextsem = 0;

    /* check all NSEM slots */
    for (sem = 0; sem < NSEM; sem++)
    {
        nextsem = (nextsem + 1) % NSEM;
        if (SFREE == semtab[nextsem].state)
        {
            semtab[nextsem].state = SUSED;
            return nextsem;
        }
    }
    return SYSERR;
}
