/**
 * @file     netaddrmask.c
 * @provides netaddrmask
 *
 * $Id: netaddrmask.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <network.h>
#include <ipv4.h>

struct netaddr NETADDR_GLOBAL_IP_BRC = { NETADDR_IPv4, IPv4_ADDR_LEN,
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
};

struct netaddr NETADDR_GLOBAL_ETH_BRC = { NETADDR_ETHERNET, ETH_ADDR_LEN,
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
};

/**
 * Apply a mask to a network address
 * @param a network address
 * @param b mask
 * @return network address with host portion masked away
 */
syscall netaddrmask(struct netaddr *a, struct netaddr *mask)
{
    int i;
    if (a->type != mask->type)
    {
        return SYSERR;
    }

    if (a->len != mask->len)
    {
        return SYSERR;
    }

    for (i = 0; i < a->len; i++)
    {
        a->addr[i] = a->addr[i] & mask->addr[i];
    }

    return OK;
}
