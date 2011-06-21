/**
 * @file semcount.c
 * @provides semcount.
 *
 * $Id: semcount.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <semaphore.h>

/**
 * Return a semaphore's count
 * @param sem    target semaphore
 * @return count from semtab entry on success, SYSERR on failure
 */
syscall semcount(semaphore sem)
{
    if (isbadsem(sem))
    {
        return SYSERR;
    }

    return (semtab[sem].count);
}
