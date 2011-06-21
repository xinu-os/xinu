/**
 * @file     netaddrsprintf.c
 * @provides netaddrsprintf
 *
 * $Id: netaddrsprintf.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <network.h>
#include <stdio.h>

/**
 * Format netaddr and place in a string
 * @param str output string
 * @param addr netaddr to format
 * @return pointer to string
 */
int netaddrsprintf(char *str, struct netaddr *addr)
{
    switch (addr->type)
    {
    case NETADDR_ETHERNET:
        sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X",
                addr->addr[0], addr->addr[1], addr->addr[2],
                addr->addr[3], addr->addr[4], addr->addr[5]);
        break;
    case NETADDR_IPv4:
        sprintf(str, "%d.%d.%d.%d",
                addr->addr[0], addr->addr[1], addr->addr[2],
                addr->addr[3]);
        break;
    case NULL:
        sprintf(str, "NULL");
        break;
    default:
        sprintf(str, "ERROR");
    }
    return (int)str;
}
