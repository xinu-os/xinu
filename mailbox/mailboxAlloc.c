/**
 * @file mailboxAlloc.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <mailbox.h>
#include <memory.h>

/**
 * @ingroup mailbox
 *
 * Allocate a mailbox that allows up to the specified number of outstanding
 * messages.
 *
 * @param count
 *      Maximum number of messages allowed for the mailbox.
 *
 * @return
 *      The index of the newly allocated mailbox, or ::SYSERR if all mailboxes
 *      are already in use or other resources could not be allocated.
 */
syscall mailboxAlloc(uint count)
{
    static uint nextmbx = 0;
    uint i;
    struct mbox *mbxptr;
    int retval = SYSERR;

    /* wait until other threads are done editing the mailbox table */
    wait(mboxtabsem);

    /* run through all mailboxes until we find a free one */
    for (i = 0; i < NMAILBOX; i++)
    {
        nextmbx = (nextmbx + 1) % NMAILBOX;
        mbxptr = &mboxtab[nextmbx];

        /* when we find a free mailbox set that one up and return it */
        if (MAILBOX_FREE == mbxptr->state)
        {
            /* get memory space for the message queue */
            mbxptr->msgs = memget(sizeof(int) * count);

            /* check if memory was allocated correctly */
            if (SYSERR == (int)mbxptr->msgs)
            {
                break;
            }

            /* initialize mailbox details and semaphores */
            mbxptr->count = 0;
            mbxptr->start = 0;
            mbxptr->max = count;
            mbxptr->sender = semcreate(count);
            mbxptr->receiver = semcreate(0);
            if ((SYSERR == (int)mbxptr->sender) ||
                (SYSERR == (int)mbxptr->receiver))
            {
                memfree(mbxptr->msgs, sizeof(int) * (mbxptr->max));
                semfree(mbxptr->sender);
                semfree(mbxptr->receiver);
                break;
            }

            /* mark this mailbox as being used */
            mbxptr->state = MAILBOX_ALLOC;

            /* return value is index of the allocated mailbox */
            retval = nextmbx;
            break;
        }
    }

    /* signal this thread is done editing the mbox tab */
    signal(mboxtabsem);

    /* return either SYSERR or the index of the allocated mailbox */
    return retval;
}
