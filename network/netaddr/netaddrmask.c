/**
 * @file     netaddrmask.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <stddef.h>
#include <network.h>
#include <ipv4.h>

const struct netaddr NETADDR_GLOBAL_IP_BRC = { NETADDR_IPv4, IPv4_ADDR_LEN,
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
};

const struct netaddr NETADDR_GLOBAL_ETH_BRC = { NETADDR_ETHERNET, ETH_ADDR_LEN,
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
};

/**
 * @ingroup network
 *
 * Apply a mask to a network address.  For example, with IPv4, masking
 * 192.168.0.50 with 255.255.255.0 will produce 192.168.0.0.
 *
 * @param a
 *      Network address to mask.
 * @param mask
 *      The mask to apply.
 *
 * @return
 *      SYSERR if @p a and @p mask are not the same type of network address;
 *      otherwise returns OK and modifies @p a in place to mask it.
 */
syscall netaddrmask(struct netaddr *a, const struct netaddr *mask)
{
    uint i;

    if (a->type != mask->type || a->len != mask->len)
    {
        return SYSERR;
    }

    for (i = 0; i < a->len; i++)
    {
        a->addr[i] &= mask->addr[i];
    }

    return OK;
}
