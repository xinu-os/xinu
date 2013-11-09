/**
 * @file mailboxFree.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <mailbox.h>
#include <memory.h>

/**
 * @ingroup mailbox
 *
 * Free the specified mailbox.
 *
 * @param box
 *      The index of the mailbox to free.
 *
 * @return
 *      ::OK if the mailbox was successfully freed, or ::SYSERR if @p box did
 *      not specify a valid allocated mailbox.
 */
syscall mailboxFree(mailbox box)
{
    struct mbox *mbxptr;
    int retval;

    if (!(0 <= box && box < NMAILBOX))
    {
        return SYSERR;
    }

    mbxptr = &mboxtab[box];

    /* wait until other threads are done editing the mailbox table */
    wait(mboxtabsem);

    if (MAILBOX_ALLOC == mbxptr->state)
    {
        /* mark mailbox as no longer allocated  */
        mbxptr->state = MAILBOX_FREE;

        /* free semaphores related to this mailbox */
        semfree(mbxptr->sender);
        semfree(mbxptr->receiver);

        /* free memory that was used for the message queue */
        memfree(mbxptr->msgs, sizeof(int) * (mbxptr->max));

        retval = OK;
    }
    else
    {
        /* mailbox was not allocated  */
        retval = SYSERR;
    }

    /* signal that this thread is done editing the mailbox table */
    signal(mboxtabsem);

    return retval;
}
