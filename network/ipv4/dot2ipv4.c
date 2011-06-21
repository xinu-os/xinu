/**
 * @file dot2ipv4.c
 * @provides dot2ipv4
 *
 * $Id: dot2ipv4.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <ctype.h>
#include <ipv4.h>
#include <network.h>

/**
 * Convert an IP Address in dotted-decimal notation into a netaddr.
 * @param str dotted-decimal IP Address string
 * @param ip netaddr structure for conversion result
 * @return OK if conversion was successful, otherwise SYSERR
 */
syscall dot2ipv4(char *str, struct netaddr *ip)
{
    char c;
    uint count = 0, temp = 0;

    /* Error check pointers */
    if ((NULL == str) || (NULL == ip))
    {
        return SYSERR;
    }

    while ((count < IPv4_ADDR_LEN) && *str)
    {
        c = *str;
        if (isdigit(c))
        {
            temp = temp * 10 + (c - '0');
        }
        else if ('.' == c)
        {
            ip->addr[count] = temp;
            temp = 0;
            count++;
        }
        else
        {
            return SYSERR;
        }
        str++;
    }
    ip->addr[count] = temp;
    ip->type = NETADDR_IPv4;
    ip->len = IPv4_ADDR_LEN;
    return OK;
}
