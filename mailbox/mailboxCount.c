/**
 * @file mailboxCount.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <interrupt.h>
#include <mailbox.h>

/**
 * @ingroup mailbox
 *
 * Count the number of messages in a mailbox.
 * @param box the number of the mailbox to count
 * @return count of messages in mailbox, SYSERR if error occured
 */
syscall mailboxCount(mailbox box)
{
    struct mbox *mbxptr;
    irqmask im;
    uint count;

    if (box >= NMAILBOX)
    {
        return SYSERR;
    }

    mbxptr = &mboxtab[box];
    if (mbxptr->state != MAILBOX_ALLOC)
    {
        return SYSERR;
    }

    im = disable();
    count = mbxptr->count;
    restore(im);

    return count;
}
