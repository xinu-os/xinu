/**
 * @file semcreate.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <semaphore.h>
#include <interrupt.h>

static semaphore semalloc(void);

/**
 * @ingroup semaphores
 *
 * Creates a semaphore that initially has the specified count.
 *
 * @param count
 *      Initial count of the semaphore (often the number of some resource that
 *      is available).  Must be non-negative.
 *
 * @return
 *      On success, returns the new semaphore; otherwise returns ::SYSERR.  The
 *      new semaphore must be freed with semfree() when no longer needed.  This
 *      function can only fail if the system is out of semaphores or if @p count
 *      was negative.
 */
semaphore semcreate(int count)
{
    semaphore sem;
    irqmask im;

    if (count < 0)          /* Don't allow starting with negative count.  */
    {
        return SYSERR;
    }

    im = disable();         /* Disable interrupts.  */
    sem = semalloc();       /* Allocate semaphore.  */
    if (SYSERR != sem)      /* If semaphore was allocated, set count.  */
    {
        semtab[sem].count = count;
    }
    /* Restore interrupts and return either the semaphore or SYSERR.  */
    restore(im);
    return sem;
}

/**
 * Allocate an unused semaphore and return its ID.
 * Scan the global semaphore table for a free entry, mark the entry
 * used, and return the new semaphore
 * @return available semaphore ID on success, SYSERR on failure
 */
static semaphore semalloc(void)
{
    int i;
    static int nextsem = 0;

    /* check all NSEM slots, starting at 1 past the last slot searched.  */
    for (i = 0; i < NSEM; i++)
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
