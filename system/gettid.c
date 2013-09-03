/**
 * @file gettid.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>

/**
 * @ingroup threads
 *
 * Get the thread id of currently executing thread.
 * @return thread id of the currently running thread
 */
tid_typ gettid(void)
{
    return thrcurrent;
}
