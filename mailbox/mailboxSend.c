/**
 * @file mailboxSend.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <stddef.h>
#include <interrupt.h>
#include <mailbox.h>

/**
 * @ingroup mailbox
 *
 * Send a message to the specified mailbox.
 *
 * @param box
 *      The index of the mailbox to send the message to.
 *
 * @param mailmsg
 *      The message to send.
 *
 * @return ::OK if the message was successfully enqueued, otherwise ::SYSERR.
 *         ::SYSERR is returned only if @p box did not specify a valid allocated
 *         mailbox or if the mailbox was freed while waiting for room in the
 *         queue.
 */
syscall mailboxSend(mailbox box, int mailmsg)
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
        /* wait until there is room in the mailmsg queue */
        wait(mbxptr->sender);

        /* only continue if the mailbox hasn't been freed  */
        if (MAILBOX_ALLOC == mbxptr->state)
        {
            /* write mailmsg to this mailbox's mailmsg queue */
            mbxptr->msgs[((mbxptr->start + mbxptr->count) % mbxptr->max)] =
                mailmsg;
            mbxptr->count++;

            /* signal that there is another mailmsg in the mailmsg queue */
            signal(mbxptr->receiver);

            retval = OK;
        }
    }

    restore(im);
    return retval;
}
