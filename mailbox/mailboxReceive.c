/**
 * @file mailboxReceive.c
 * @provides mailboxReceive.
 *
 * $Id: mailboxReceive.c 2021 2009-08-13 17:55:30Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <interrupt.h>
#include <mailbox.h>

/**
 * Receive a mailmsg from a mailbox.
 * @param box the number of the mailbox to receive from
 * @return the mailmsg that was dequeued, otherwise SYSERR
 */
syscall mailboxReceive(mailbox box)
{
    struct mbox *mbxptr;
    int mailmsg;
    irqmask im;

    if (box >= NMAILBOX)
    {
        return SYSERR;
    }

    mbxptr = &mboxtab[box];
    if (MAILBOX_ALLOC != mbxptr->state)
    {
        return SYSERR;
    }

    /* wait until there is a mailmsg in the mailmsg queue */
    wait(mbxptr->receiver);

    im = disable();

    /* recieve the first mailmsg in the mailmsg queue */
    mailmsg = mbxptr->msgs[mbxptr->start];

    mbxptr->start = (mbxptr->start + 1) % mbxptr->max;
    mbxptr->count--;

    restore(im);

    /* signal that there is another empty space in the mailmsg queue */
    signal(mbxptr->sender);

    return mailmsg;
}
