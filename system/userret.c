/**
 * @file userret.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>

/**
 * @ingroup threads
 *
 * Entered when a thread exits by return.
 */
void userret(void)
{
    kill(gettid());
}
