/**
 * @file moncount.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <semaphore.h>
#include <monitor.h>

/**
 * @ingroup monitors
 *
 * Return a monitor's count
 * @param mon    target monitor
 * @return count from montab entry on success, SYSERR on failure
 */
syscall moncount(monitor mon)
{
    if (isbadmon(mon))
    {
        return SYSERR;
    }

    return (montab[mon].count);
}
