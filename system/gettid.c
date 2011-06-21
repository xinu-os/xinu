/**
 * @file gettid.c
 * @provides gettid.
 *
 * $Id: gettid.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>

/**
 * Get the thread id of currently executing thread.
 * @return thread id of the currently running thread
 */
tid_typ gettid(void)
{
    return thrcurrent;
}
