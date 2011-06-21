/**
 * file ipv4RecvValid.c
 * @provides ipv4RecvValid
 * 
 * $Id: ipv4RecvValid.c 2020 2009-08-13 17:50:08Z mschul $
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
    /* Validate checksum */
    if (netChksum(ip, (4 * (ip->ver_ihl & IPv4_IHL))) != 0)
    {
        return FALSE;
    }

    /* Check version */
    if (((ip->ver_ihl & IPv4_VER) >> 4) != IPv4_VERSION)
    {
        return FALSE;
    }


    /* Check total length */
    if (ip->len < (IPv4_MIN_IHL * 4))
    {
        return FALSE;
    }

    /* Check length of header */
    if (((ip->ver_ihl & IPv4_IHL) < IPv4_MIN_IHL)
        || ((ip->ver_ihl & IPv4_IHL) > IPv4_MAX_IHL))
    {
        return FALSE;
    }

    return TRUE;

}
