/**
 * @file semcount.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <semaphore.h>

/**
 * @ingroup semaphores
 *
 * Retrieve a semaphore's count.
 *
 * @param sem
 *      Semaphore to get the count of.
 *
 * @return
 *      On success, returns the semaphore's count; otherwise returns ::SYSERR.
 *      This function can only fail if @p sem did not specify a valid semaphore.
 */
syscall semcount(semaphore sem)
{
    if (isbadsem(sem))
    {
        return SYSERR;
    }

    return (semtab[sem].count);
}
