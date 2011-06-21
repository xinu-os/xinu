/*
 * @file ethernet.h
 *
 * $Id: ethernet.h 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _ETHERNET_H_
#define _ETHERNET_H_

#include <stddef.h>

/* Ether Packet Types */
#define ETHER_TYPE_IPv4   0x0800
#define ETHER_TYPE_ARP    0x0806

#define ETH_ADDR_LEN	6
#define ETH_HDR_LEN		14

/*
 * ETHERNET HEADER
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Destination MAC                                               |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Destination MAC               | Source MAC                    |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Source MAC                                                    |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Ether Type                    | Data                          |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Data (46 - 1500 octets)                                       |
 * | ...                                                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

/* Ethernet packet header */
struct etherPkt
{
    uchar dst[ETH_ADDR_LEN];    /* Destination Mac */
    uchar src[ETH_ADDR_LEN];    /* Source Mac      */
    ushort type;                /* Ether type      */
    uchar data[1];              /* Packet data     */
};

#endif                          /* _ETHERNET_H_ */
