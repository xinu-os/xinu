/**
 * @file     netaddrequal.c
 * @provides netaddrequal
 *
 * $Id: netaddrequal.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <network.h>
#include <string.h>

/**
 * Compare to network addresses
 * @param a first network address
 * @param b second network address
 * @return TRUE if addresses are the same, otherwise FALSE
 */
bool netaddrequal(struct netaddr *a, struct netaddr *b)
{
    if (a->type != b->type)
    {
        return FALSE;
    }

    if (a->len != b->len)
    {
        return FALSE;
    }

    if (0 != memcmp(a->addr, b->addr, a->len))
    {
        return FALSE;
    }

    return TRUE;
}
