/**
 * @file     netaddrhost.c
 * @provides netaddrhost
 *
 * $Id: netaddrhost.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <network.h>

/**
 * Return host portion of network address
 * @param a network address
 * @param b mask
 * @return host portion of network address
 */
syscall netaddrhost(struct netaddr *a, struct netaddr *mask)
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
        a->addr[i] = a->addr[i] & ~mask->addr[i];
    }

    return OK;
}
