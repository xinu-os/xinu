/**
 * @file arpAlloc.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <arp.h>
#include <stdlib.h>

/**
 * @ingroup arp
 *
 * Allocates an entry from the ARP table.
 * @return entry in ARP table, SYSERR if error occurs
 * @pre-condition interrupts are disabled
 * @post-condition interrupts are still disabled
 */
struct arpEntry *arpAlloc(void)
{
    struct arpEntry *minexpires = NULL;
    int i = 0;

    ARP_TRACE("Allocating ARP entry");

    for (i = 0; i < ARP_NENTRY; i++)
    {
        /* If entry is free, return entry */
        if (ARP_FREE == arptab[i].state)
        {
            arptab[i].state = ARP_USED;
            ARP_TRACE("\tFree entry %d", i);
            return &arptab[i];
        }

        if ((NULL == minexpires)
            || (arptab[i].expires < minexpires->expires))
        {
            minexpires = &arptab[i];
            ARP_TRACE("\tMinexpires entry %d, expires %u", i,
                      minexpires->expires);
        }
    }

    /* If no free or minimum expires entry was found an error occured */
    if (NULL == minexpires)
    {
        ARP_TRACE("\tNo free or minexpires entry");
        return (struct arpEntry *)SYSERR;
    }

    /* Return entry with minimum expires */
    bzero(minexpires, sizeof(struct arpEntry));
    minexpires->state = ARP_USED;
    return minexpires;
}
