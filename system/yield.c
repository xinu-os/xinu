/**
 * @file yield.c
 * @provides yield.
 *
 * $Id: yield.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>

/**
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
