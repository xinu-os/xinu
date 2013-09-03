/**
 * @file     netaddrequal.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <network.h>
#include <string.h>

/**
 * @ingroup network
 *
 * Compare two network addresses
 * @param a first network address
 * @param b second network address
 * @return ::TRUE if addresses are the same, otherwise ::FALSE.
 */
bool netaddrequal(const struct netaddr *a, const struct netaddr *b)
{
    return (a->type == b->type &&
            a->len == b->len &&
            0 == memcmp(a->addr, b->addr, a->len));
}
