/**
 * @file rtLookup.c
 * 
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <interrupt.h>
#include <network.h>
#include <route.h>

/**
 * @ingroup route
 *
 * Looks up an entry in the routing table
 * @param addr the IP address that needs routing
 * @return a route table entry, NULL if none matches, SYSERR on error
 */
struct rtEntry *rtLookup(const struct netaddr *addr)
{
    int i;
    struct rtEntry *rtptr;
    struct netaddr masked;
    irqmask im;

    rtptr = NULL;

    RT_TRACE("Addr = %d.%d.%d.%d", addr->addr[0], addr->addr[1],
             addr->addr[2], addr->addr[3]);

    im = disable();
    for (i = 0; i < RT_NENTRY; i++)
    {
        if (RT_USED == rttab[i].state)
        {
            /* Mask off address */
            netaddrcpy(&masked, addr);
            netaddrmask(&masked, &rttab[i].mask);

            /* Check if match  */
            if (netaddrequal(&masked, &rttab[i].dst))
            {
                RT_TRACE("Matched entry %d", i);
                /* Remember match if no match so far or match is better */
                if ((NULL == rtptr)
                    || (rtptr->masklen < rttab[i].masklen))
                {
                    rtptr = &rttab[i];
                }
            }
        }
    }
    restore(im);

    return rtptr;
}
