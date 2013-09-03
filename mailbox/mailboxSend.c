/**
 * @file mailboxSend.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <interrupt.h>
#include <mailbox.h>

/**
 * @ingroup mailbox
 *
 * Send a mailmsg to a mailbox.
 * @param box the number of the mailbox to send to
 * @param mailmsg the mailmsg to send
 * @return OK if the mailmsg was enqueued, otherwise SYSERR
 */
syscall mailboxSend(mailbox box, int mailmsg)
{
    struct mbox *mbxptr;
    irqmask im;

    if (box >= NMAILBOX)
    {
        return SYSERR;
    }

    mbxptr = &mboxtab[box];
    if (mbxptr->state != MAILBOX_ALLOC)
    {
        return SYSERR;
    }

    /* wait until there is room in the mailmsg queue */
    wait(mbxptr->sender);

    im = disable();

    /* write mailmsg to this mailbox's mailmsg queue */
    mbxptr->msgs[((mbxptr->start + mbxptr->count) % mbxptr->max)] =
        mailmsg;
    mbxptr->count++;

    restore(im);

    /* signal that there is another mailmsg in the mailmsg queue */
    signal(mbxptr->receiver);

    return OK;
}
