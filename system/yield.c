/**
 * @file yield.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>

/**
 * @ingroup threads
 *
 * Yield processor.
 * @return OK when the thread is context switched back
 */
syscall yield(void)
{
    irqmask im;

    im = disable();
    resched();
    restore(im);
    return OK;
}
