/**
 * @file arpNotify.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <arp.h>
#include <interrupt.h>
#include <stdlib.h>
#include <thread.h>

/**
 * @ingroup arp
 *
 * Send a message to threads waiting on resolution of an ARP table entry.
 * @param entry ARP table entry
 * @param msg message to send to waiting threads
 * @return OK if successful, SYSERR if error occurs
 */
syscall arpNotify(struct arpEntry *entry, message msg)
{
    int i = 0;                          /**< index into ARP table         */
    irqmask im;                     /**< interrupt state              */

    /* Error check pointers */
    if (NULL == entry)
    {
        return SYSERR;
    }

    /* Send message to each waiting thread */
    im = disable();
    for (i = 0; i < entry->count; i++)
    {
        if (SYSERR == send(entry->waiting[i], msg))
        {
            restore(im);
            return SYSERR;
        }
    }

    /* Clear list of waiting threads */
    entry->count = 0;
    bzero(entry->waiting, sizeof(tid_typ) * ARP_NTHRWAIT);

    restore(im);
    return OK;
}
