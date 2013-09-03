/**
 * @file mailboxFree.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <mailbox.h>
#include <memory.h>
#include <semaphore.h>

/**
 * @ingroup mailbox
 *
 * Free a mailbox.
 * @param box the number of the mailbox to delete
 * @return OK if the mailbox was deleted successfully, otherwise SYSERR
 */
syscall mailboxFree(mailbox box)
{
    struct mbox *mbxptr;

    if (box >= NMAILBOX)
    {
        return SYSERR;
    }

    mbxptr = &mboxtab[box];
    if (MAILBOX_ALLOC != mbxptr->state)
    {
        return SYSERR;
    }

    /* wait until other threads are done editing the mailbox table */
    wait(mboxtabsem);

    /* free semaphores related to this mailbox */
    if ((SYSERR == (int)semfree(mbxptr->sender))
        || (SYSERR == (int)semfree(mbxptr->receiver)))
    {
        /* signal and return SYSERR */
        signal(mboxtabsem);
        return SYSERR;
    }

    /* free memory that was used for the message queue */
    if (SYSERR == memfree(mbxptr->msgs, sizeof(int) * (mbxptr->max)))
    {
        /* signal and return SYSERR */
        signal(mboxtabsem);
        return SYSERR;
    }

    /* free this mailbox for use */
    mbxptr->state = MAILBOX_FREE;

    /* signal that this thread is done editing the mailbox table */
    signal(mboxtabsem);

    return OK;
}
