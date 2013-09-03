/**
 * @file moncreate.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <monitor.h>
#include <semaphore.h>
#include <interrupt.h>

static monitor monalloc(void);

/**
 * @ingroup monitors
 *
 * Create and initialize a monitor, returning its ID.
 * @param count  resources available.
 * @return new monitor ID on success, SYSERR on failure
 */
monitor moncreate(int count)
{
    //register semaphore sem;
    register monitor mon;
    irqmask im;

    im = disable();             /* disable interrupts    */
    mon = monalloc();           /* request new monitor */
    if (mon != SYSERR && count >= 0)    /* safety check          */
    {
        //probably dont have to initialize to 0 since all monitors
        //will always start with their count equal to 0
        montab[mon].count = 0;
        montab[mon].owner = NOOWNER;    /* no one owns a new monitor */
        montab[mon].sem = semcreate(count);     /* initialize semaphore  */
        restore(im);            /* restore interrupts    */
        return mon;             /* return monitor id   */
    }

    restore(im);
    return SYSERR;
}

/**
 * Allocate an unused monitor and return its ID.
 * Scan the global monitor table for a free entry, mark the entry
 * used, and return the new monitor
 * @return available monitor ID on success, SYSERR on failure
 */
static monitor monalloc(void)
{
#if NMON
    int mon = 0;                /* monitor to return */
    static int nextmon = 0;

    /* check all NMON slots */
    for (mon = 0; mon < NMON; mon++)
    {
        nextmon = (nextmon + 1) % NMON;
        if (MFREE == montab[nextmon].state)
        {
            montab[nextmon].state = MUSED;
            return nextmon;
        }
    }
#endif                          /* NMON */
    return SYSERR;
}
