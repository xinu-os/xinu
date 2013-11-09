/**
 * @file mailboxReceive.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <interrupt.h>
#include <mailbox.h>

/**
 * @ingroup mailbox
 *
 * Receive a message from the specified mailbox.
 *
 * @param box
 *      The index of the mailbox to receive the message from.
 *
 * @return
 *      On success, returns the message that was dequeued; on failure (@p box
 *      did not specify an allocated mailbox, or the mailbox was freed while
 *      waiting for a message) returns ::SYSERR.  Note that it may be impossible
 *      to disambiguate ::SYSERR from a successful return value.
 */
syscall mailboxReceive(mailbox box)
{
    struct mbox *mbxptr;
    irqmask im;
    int retval;

    if (!(0 <= box && box < NMAILBOX))
    {
        return SYSERR;
    }

    mbxptr = &mboxtab[box];
    im = disable();
    retval = SYSERR;
    if (MAILBOX_ALLOC == mbxptr->state)
    {
        /* wait until there is a mailmsg in the mailmsg queue */
        wait(mbxptr->receiver);

        /* only continue if the mailbox hasn't been freed  */
        if (MAILBOX_ALLOC == mbxptr->state)
        {
            /* recieve the first mailmsg in the mailmsg queue */
            retval = mbxptr->msgs[mbxptr->start];

            mbxptr->start = (mbxptr->start + 1) % mbxptr->max;
            mbxptr->count--;

            /* signal that there is another empty space in the mailmsg queue */
            signal(mbxptr->sender);
        }
    }

    restore(im);
    return retval;
}
