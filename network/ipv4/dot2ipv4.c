/**
 * @file dot2ipv4.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <ipv4.h>
#include <network.h>
#include <stdio.h>

/**
 * @ingroup ipv4
 *
 * Convert an IPv4 address in dotted-decimal notation into a netaddr.
 *
 * @param str
 *      The dotted-decimal IPv4 address string to convert.
 * @param ip
 *      The netaddr structure for conversion result.
 *
 * @return
 *      ::OK if conversion was successful, otherwise ::SYSERR.
 */
syscall dot2ipv4(const char *str, struct netaddr *ip)
{
    uint o0, o1, o2, o3;
    char tmp;

    /* Error check pointers */
    if ((NULL == str) || (NULL == ip))
    {
        return SYSERR;
    }

    if (4 != sscanf(str, "%3u.%3u.%3u.%3u%c", &o0, &o1, &o2, &o3, &tmp) ||
        o0 > 0xff || o1 > 0xff || o2 > 0xff || o3 > 0xff)
    {
        return SYSERR;
    }

    ip->addr[0] = o0;
    ip->addr[1] = o1;
    ip->addr[2] = o2;
    ip->addr[3] = o3;
    ip->type = NETADDR_IPv4;
    ip->len = IPv4_ADDR_LEN;
    return OK;
}
