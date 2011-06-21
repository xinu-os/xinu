/**
 * @file userret.c
 * @provides userret.
 *
 * $Id: userret.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <thread.h>

/**
 * Entered when a thread exits by return.
 */
void userret(void)
{
    kill(gettid());
}
