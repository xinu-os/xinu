/**
 * @file dhcp.h
 *
 */
/* Embedded Xinu, Copyright (C) 2008, 2013.  All rights reserved. */

#ifndef _DHCP_H_
#define _DHCP_H_

#include <stddef.h>
#include <network.h>
#include <dhcpc.h>

/* DHCP Operations */
#define DHCP_OP_REQUEST		1
#define DHCP_OP_REPLY		2


/* DHCP Timeout definitions */
#define DHCP_RETRANSMIT_COUNT    4
#define DHCP_RETRANSMIT_TIME     5000   /* in ms */

/* DHCP Client States  */
#define DHCPC_STATE_INIT         0
#define DHCPC_STATE_SELECTING    1
#define DHCPC_STATE_REQUESTING   2
#define DHCPC_STATE_BOUND        3
#define DHCPC_STATE_RENEW        4
#define DHCPC_STATE_REBIND       5

/* DHCP Message Types */
#define DHCPDISCOVER    1
#define DHCPOFFER		2
#define DHCPREQUEST		3
#define DHCPDECLINE		4
#define DHCPACK			5
#define DHCPNAK			6
#define DHCPRELEASE		7
#define DHCP_TIMEOUT	0

/* DHCP definitions */
#define DHCP_HDR_LEN		240
#define DHCP_OMSGTYPE_LEN	3
#define DHCP_MAGICCOOKIE    0x63825363
#define DHCP_HTYPE_ETHER	1
#define DHCP_BROADCAST		0x8000

/* DHCP OPTIONS */
#define DHCP_OPT_END            255
#define DHCP_OPT_PAD            0
#define DHCP_OPT_SUBNET         1
#define DHCP_OPT_GATEWAY        3
#define DHCP_OPT_DNS            6
#define DHCP_OPT_HNAME			12
#define DHCP_OPT_DOMAIN         15
#define DHCP_OPT_REQUEST        50
#define DHCP_OPT_LEASE          51
#define DHCP_OPT_MSGTYPE        53
#define DHCP_OPT_SERVER         54
#define DHCP_OPT_PARAMREQ		55

/*
 * DHCP PACKET
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Ethernet Header (14 octets)                                   |
 * | ...                                                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | IP Header (20 octets)                                         |
 * | ...                                                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | UDP Header (8 octets)                                         |
 * | ...                                                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Op            | Hardware Type | Hardware Len  | Hops          |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Transaction Id                                                |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Seconds                       | Flags                         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Client IP                                                     |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Your IP                                                       |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Server IP                                                     |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Gateway IP                                                    |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Client Hardware (16 octets)                                   |
 * | ...                                                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Server Hostname (64 octets)                                   |
 * | ...                                                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Boot filename (128 octets)                                    |
 * | ...                                                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | DHCP Options (Variable octets)                                |
 * | Each options has: Type (1 octet), Len (1 octet), and Value    |
 * | ...                                                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

struct dhcpPkt
{
    uchar op;             /**< Operation (request or reply) */
    uchar htype;          /**< Hardware type (ethernet)     */
    uchar hlen;           /**< Hardware length              */
    uchar hops;           /**< Hops                         */

    uint xid;             /**< Transfer Identification      */
    ushort secs;          /**< Seconds still start time     */
    ushort flags;         /**< Flags                        */

    uint ciaddr;          /**< Client ip address            */
    uint yiaddr;          /**< Your ip address              */
    uint siaddr;          /**< Server ip address            */
    uint giaddr;          /**< Gateway ip address           */
    uchar chaddr[16];     /**< Client hardware address      */

    char sname[64];       /**< Server name                  */
    char file[128];       /**< File (for extension)         */
    uint cookie;          /**< Magic cookie                 */
    uchar opts[1];        /**< Options                      */
};


//#define ENABLE_DHCP_TRACE

#ifdef ENABLE_DHCP_TRACE
#  include <stdio.h>
#  include <thread.h>
#  define DHCP_TRACE(format, ...)                                     \
do                                                                    \
{                                                                     \
    fprintf(stderr, "%s:%d (%d) ", __FILE__, __LINE__, gettid());     \
    fprintf(stderr, format, ## __VA_ARGS__);                          \
    fprintf(stderr, "\n");                                            \
} while (0)
#else
#  define DHCP_TRACE(format, ...)
#endif


/* Note: dhcpClient() is declared in dhcpc.h  */
syscall dhcpSendRequest(int descrp, struct dhcpData *data);
syscall dhcpRecvReply(int descrp, struct dhcpData *data, uint timeout);

#endif /* _DHCP_H_ */
