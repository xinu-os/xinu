/**
 * @file arpFree.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <arp.h>
#include <interrupt.h>
#include <stdlib.h>

/**
 * @ingroup arp
 *
 * Frees an entry from the ARP table.
 * @return SYSERR if error occurs, otherwise OK
 */
syscall arpFree(struct arpEntry *entry)
{
    ARP_TRACE("Freeing ARP entry");

    /* Error check pointers */
    if (NULL == entry)
    {
        return SYSERR;
    }

    /* Notify threads waiting on resolution */
    if (ARP_UNRESOLVED == entry->state)
    {
        arpNotify(entry, TIMEOUT);
        ARP_TRACE("Waiting threads notified");
    }

    /* Clear ARP table entry */
    bzero(entry, sizeof(struct arpEntry));
    entry->state = ARP_FREE;
    ARP_TRACE("Freed entry %d",
              ((int)entry - (int)arptab) / sizeof(struct arpEntry));
    return OK;
}
