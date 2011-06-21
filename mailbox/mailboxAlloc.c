/**
 * @file mailboxAlloc.c
 * @provides mailboxAlloc.
 *
 * $Id: mailboxAlloc.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <mailbox.h>
#include <memory.h>

/**
 * Allocate a mailbox that allows count outstanding messages.
 * @param count maximum number of messages allowed for the mailbox
 * @return the number of the mailbox, SYSERR if none are available
 */
syscall mailboxAlloc(uint count)
{
    static int nextmbx = 0;
    struct mbox *mbxptr;
    ushort i = 0;

    /* wait until other threads are done editing the mailbox table */
    wait(mboxtabsem);

    /* run through all mailboxes until we find a free one */
    for (i = 0; i < NMAILBOX; i++)
    {
        nextmbx = (nextmbx + 1) % NMAILBOX;
        /* when we find a free mailbox set that one up and return it */
        if (MAILBOX_FREE == mboxtab[nextmbx].state)
        {
            mbxptr = &mboxtab[nextmbx];

            /* get memory space for the message queue */
            mbxptr->msgs = memget(sizeof(int) * count);

            /* check if memory was allocated correctly */
            if (SYSERR == (int)mbxptr->msgs)
            {
                /* signal and return SYSERR */
                signal(mboxtabsem);
                return SYSERR;
            }

            /* initialize mailbox details and semaphores */
            mbxptr->max = count;

            mbxptr->sender = semcreate(count);
            mbxptr->receiver = semcreate(0);
            if ((SYSERR ==
                 (int)mbxptr->sender)
                || (SYSERR == (int)mbxptr->receiver))
            {
                memfree(mbxptr->msgs, sizeof(int) * (mbxptr->max));
                semfree(mbxptr->sender);
                semfree(mbxptr->receiver);
                return SYSERR;
            }
            mbxptr->count = 0;
            mbxptr->start = 0;

            /* mark this mailbox as being used */
            mbxptr->state = MAILBOX_ALLOC;

            /* signal this thread is done editing the mbox tab */
            signal(mboxtabsem);

            /* return the this entry in the mboxtab */
            return nextmbx;
        }
    }

    /* signal and return SYSERR because we didn't find a free spot */
    signal(mboxtabsem);
    return SYSERR;
}
