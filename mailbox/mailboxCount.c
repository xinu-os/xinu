/**
 * @file mailboxCount.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <interrupt.h>
#include <mailbox.h>

/**
 * @ingroup mailbox
 *
 * Retrieve the number of outstanding messages in the specified mailbox.
 *
 * @param box
 *      The index of the mailbox for which to retrieve the number of outstanding
 *      messages.
 *
 * @return
 *      The number of messages in the mailbox, or ::SYSERR if @p box did not
 *      specify a valid allocated mailbox.
 */
syscall mailboxCount(mailbox box)
{
    const struct mbox *mbxptr;
    irqmask im;
    int retval;

    if (!(0 <= box && box < NMAILBOX))
    {
        return SYSERR;
    }

    mbxptr = &mboxtab[box];
    im = disable();
    if (MAILBOX_ALLOC == mbxptr->state)
    {
        retval = mbxptr->count;
    }
    else
    {
        retval = SYSERR;
    }
    restore(im);
    return retval;
}
