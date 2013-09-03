/**
 * file ipv4RecvValid.c
 * 
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <ipv4.h>

/**
 * Verifies ip header is valid and sends necessary bad param messages.
 * @param ip the ip packet to verify
 * @return TRUE if the packet is valid, otherwise false
 */
bool ipv4RecvValid(struct ipv4Pkt *ip)
{
	int ip_ihl = ip->ver_ihl & IPv4_IHL;

    /* Check version */
    if (((ip->ver_ihl & IPv4_VER) >> 4) != IPv4_VERSION)
    {
        return FALSE;
    }

    /* Check length of header */
    if ((ip_ihl < IPv4_MIN_IHL) || (ip_ihl > IPv4_MAX_IHL))
    {
        return FALSE;
    }

    /* Check total length */
    if (ip->len < (IPv4_MIN_IHL * 4))
    {
        return FALSE;
    }

    /* Validate checksum */
    if (netChksum(ip, (ip_ihl * 4)) != 0)
    {
        return FALSE;
    }

    return TRUE;
}
