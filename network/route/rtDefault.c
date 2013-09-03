/** 
 * @file rtDefault.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <network.h>
#include <route.h>
#include <stdlib.h>

/**
 * @ingroup route
 *
 * Set the default route.
 * @param gate gateway for default route
 * @param nif network interface for default route
 * @return OK if added/updated successfully, otherwise SYSERR
 */
syscall rtDefault(const struct netaddr *gate, struct netif *nif)
{
    struct rtEntry *rtptr;
    int i;
    struct netaddr mask;

    /* Error check pointers */
    if ((NULL == gate) || (NULL == nif) || (gate->len > NET_MAX_ALEN))
    {
        RT_TRACE("Invalid args");
        return SYSERR;
    }

    /* Setup mask for default route */
    mask.type = gate->type;
    mask.len = gate->len;
    bzero(mask.addr, mask.len);

    /* Check if a default route already exists */
    rtptr = NULL;
    for (i = 0; i < RT_NENTRY; i++)
    {
        if ((RT_USED == rttab[i].state)
            && (netaddrequal(&rttab[i].mask, &mask)))
        {
            RT_TRACE("Default route exists, entry %d", i);
            return OK;
        }
    }

    /* Allocate an entry in the route table */
    rtptr = rtAlloc();
    if ((SYSERR == (int)rtptr) || (NULL == rtptr))
    {
        RT_TRACE("Failed to allocate route entry");
        return SYSERR;
    }

    /* Populate the entry */
    netaddrcpy(&rtptr->dst, &mask);
    netaddrcpy(&rtptr->gateway, gate);
    netaddrcpy(&rtptr->mask, &mask);
    rtptr->nif = nif;

    /* Calculate mask length */
    rtptr->masklen = 0;

    rtptr->state = RT_USED;
    RT_TRACE("Populated default route");
    return OK;
}
