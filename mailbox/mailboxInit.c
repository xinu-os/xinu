/**
 * @file mailboxInit.c
 * @provides mailboxInit.
 *
 * $Id: mailboxInit.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <mailbox.h>

struct mbox mboxtab[NMAILBOX];
semaphore mboxtabsem;

/**
 * Initialize mailbox structures.
 * @return OK if all mailboxes are initialized successfully
 */
syscall mailboxInit(void)
{
    int i = 0;

    mboxtabsem = semcreate(1);

    /* set all mailbox states to MAILBOX_FREE */
    for (i = 0; i < NMAILBOX; i++)
    {
        mboxtab[i].state = MAILBOX_FREE;
    }

    return OK;
}
