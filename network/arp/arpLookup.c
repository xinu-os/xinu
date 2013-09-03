/**
 * @file arpLookup.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <arp.h>
#include <clock.h>
#include <interrupt.h>
#include <string.h>
#include <thread.h>

/**
 * @ingroup arp
 *
 * Obtains a hardware address from the ARP table given a protocol address.
 * @param netptr network interface
 * @param praddr protocol address
 * @param hwaddr buffer into which hardware address should be placed
 * @return OK if hardware address was obtained, otherwise TIMEOUT or SYSERR
 */
syscall arpLookup(struct netif *netptr, const struct netaddr *praddr,
                  struct netaddr *hwaddr)
{
    struct arpEntry *entry = NULL;  /**< pointer to ARP table entry   */
    uint lookups = 0;                   /**< num of ARP lookups performed */
    int ttl;                            /**< TTL for ARP table entry      */
    irqmask im;                         /**< interrupt state              */

    /* Error check pointers */
    if ((NULL == netptr) || (NULL == praddr) || (NULL == hwaddr))
    {
        ARP_TRACE("Invalid args");
        return SYSERR;
    }

    ARP_TRACE("Looking up protocol address");

    /* Attempt to obtain destination hardware address from ARP table until:
     * 1) lookup succeeds; 2) TIMEOUT occurs; 3) SYSERR occurs; or
     * 4) maximum number of lookup attempts occrus. */
    while (lookups < ARP_MAX_LOOKUP)
    {
        lookups++;

        /* Obtain entry from ARP table */
        im = disable();
        entry = arpGetEntry(praddr);

        /* If ARP entry does not exist; create an unresolved entry */
        if (NULL == entry)
        {
            ARP_TRACE("Entry does not exist");
            entry = arpAlloc();
            if (SYSERR == (int)entry)
            {
                restore(im);
                return SYSERR;
            }

            entry->state = ARP_UNRESOLVED;
            entry->nif = netptr;
            netaddrcpy(&entry->praddr, praddr);
            entry->expires = clktime + ARP_TTL_UNRESOLVED;
            entry->count = 0;
        }

        /* Place hardware address in buffer if entry is resolved */
        if (ARP_RESOLVED == entry->state)
        {
            netaddrcpy(hwaddr, &entry->hwaddr);
            ARP_TRACE("Entry exists");
            return OK;
        }

        /* Entry is unresolved; enqueue thread to wait for resolution */
        if (entry->count >= ARP_NTHRWAIT)
        {
            restore(im);
            ARP_TRACE("Queue of waiting threads is full");
            return SYSERR;
        }
        entry->waiting[entry->count] = gettid();
        entry->count++;
        ttl = (entry->expires - clktime) * CLKTICKS_PER_SEC;
        restore(im);

        /* Send an ARP request and wait for response */
        if (SYSERR == arpSendRqst(entry))
        {
            ARP_TRACE("Failed to send request");
            return SYSERR;
        }
        recvclr();
        switch (recvtime(ttl))
        {
        case TIMEOUT:
        case SYSERR:
            return SYSERR;
        case ARP_MSG_RESOLVED:
        default:
            /* Reply received, address resolved, re-attempt lookup */
            continue;
        }
    }

    return SYSERR;
}
