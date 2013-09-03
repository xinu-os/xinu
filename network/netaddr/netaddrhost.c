/**
 * @file     netaddrhost.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <stddef.h>
#include <network.h>

/**
 * @ingroup network
 *
 * Calculate the host portion of a network address, which is essentially the
 * opposite of masking it with netaddrmask().  For example, with IPv4,
 * extracting the host portion of 192.168.0.50 with a mask of 255.255.255.0 will
 * produce 0.0.0.50.

 * @param a
 *      Network address from which to extract the host portion.
 * @param mask
 *      The netmask to use.
 *
 * @return
 *      SYSERR if @p a and @p mask are not the same type of network address;
 *      otherwise returns OK and modifies @p a in place to produce just the host
 *      portion of the network address.
 */
syscall netaddrhost(struct netaddr *a, const struct netaddr *mask)
{
    uint i;

    if (a->type != mask->type || a->len != mask->len)
    {
        return SYSERR;
    }

    for (i = 0; i < a->len; i++)
    {
        a->addr[i] &= ~mask->addr[i];
    }

    return OK;
}
