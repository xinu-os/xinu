/**
 * @file mailboxInit.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <mailbox.h>

struct mbox mboxtab[NMAILBOX];
semaphore mboxtabsem;

/**
 * @ingroup mailbox
 *
 * Initialize mailbox structures.
 *
 * @return
 *      ::OK if all mailboxes were initialized successfully, otherwise ::SYSERR.
 */
syscall mailboxInit(void)
{
    uint i;

    /* set all mailbox states to MAILBOX_FREE */
    for (i = 0; i < NMAILBOX; i++)
    {
        mboxtab[i].state = MAILBOX_FREE;
    }

    mboxtabsem = semcreate(1);
    if (SYSERR == mboxtabsem)
    {
        return SYSERR;
    }

    return OK;
}
