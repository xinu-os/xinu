/*
 * @file ipv4.h
 *
 * $Id: ipv4.h 2037 2009-08-13 23:00:27Z agember $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _IPv4_H_
#define _IPv4_H_

#include <stddef.h>
#include <network.h>

/* Tracing macros */
//#define TRACE_IPv4     TTY1
#ifdef TRACE_IPv4
#include <stdio.h>
#define IPv4_TRACE(...)     { \
		fprintf(TRACE_IPv4, "%s:%d (%d) ", __FILE__, __LINE__, gettid()); \
		fprintf(TRACE_IPv4, __VA_ARGS__); \
		fprintf(TRACE_IPv4, "\n"); }
#else
#define IPv4_TRACE(...)
#endif

/* Maximum length of an IPv4 address in dot-decimal notation */
#define IPv4_DOTDEC_MAXLEN    15

/* Masks to point to section of packet */
#define IPv4_IHL			0x0F
#define IPv4_VER			0xF0
#define IPv4_FLAGS		0xE000
#define IPv4_FROFF		0x1FFF
#define IPv4_ADDR_LEN       4
#define IPv4_TTL 			64

/* IP packet definitions */
#define IPv4_HDR_LEN     	20      /* (Assumes no options or padding) */
#define IPv4_MAX_OPTLEN		40
#define IPv4_MAX_HDRLEN 	IPv4_HDR_LEN + IPv4_MAX_OPTLEN
#define IPv4_MIN_IHL		5
#define IPv4_MAX_IHL		15
#define IPv4_VERSION		4

/* IP Protocols */
#define IPv4_PROTO_ICMP  1
#define IPv4_PROTO_IGMP  2
#define IPv4_PROTO_TCP	 6
#define IPv4_PROTO_UDP   17
#define IPv4_PROTO_SCTP  132

/* Flags */
#define IPv4_FLAG_LF		0x0000
#define IPv4_FLAG_MF 		0x2000
#define IPv4_FLAG_DF 		0x4000

/* Types of service */
#define IPv4_TOS_NETCNTRL	0x7
#define IPv4_TOS_INTCNTRL	0x6
#define IPv4_TOS_CRITIC	0x5
#define IPv4_TOS_FLASHOV	0x4
#define IPv4_TOS_FLASH	0x3
#define IPv4_TOS_IM		0x2
#define IPv4_TOS_PRIO		0x1
#define IPv4_TOS_ROUTINE	0x0

/*
 * IPv4 HEADER
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Link-Level Header                                             |
 * | ...                                                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |Version| IHL   |Type of Service| Total Length (IHL + Data)     |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |         Identification        |Flags|    Fragment Offset      |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Time to Live  |     Protocol  |       Header Checksum         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                         Source Address                        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                       Destination Address                     |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |              Options  & Padding (Variable octets)             |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Data (Variable octets)                                        |
 * | ...                                                           | 
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

struct ipv4Pkt             /**< IPv4 Packet Variables                   */
{
    uchar ver_ihl;           /**< IPv4 Version and Internet Header Len  */
    uchar tos;               /**< IPv4 Type of Service                  */
    ushort len;              /**< IPv4 packet length including IHL      */
    ushort id;               /**< IPv4 identification                   */
    ushort flags_froff;      /**< IPv4 Flags and Fragment offset        */
    uchar ttl;               /**< IPv4 time to live                     */
    uchar proto;             /**< IPv4 protocol                         */
    ushort chksum;           /**< IPv4 checksum                         */
    uchar src[IPv4_ADDR_LEN];  /**< IPv4 source                           */
    uchar dst[IPv4_ADDR_LEN];  /**< IPv4 destination                      */
    uchar opts[1];           /**< Options and padding is variable       */
};

/* Function prototypes */
syscall dot2ipv4(char *, struct netaddr *);
syscall ipv4Recv(struct packet *);
bool ipv4RecvValid(struct ipv4Pkt *);
bool ipv4RecvDemux(struct netaddr *);
syscall ipv4Send(struct packet *, struct netaddr *, struct netaddr *,
                 uchar);
syscall ipv4SendFrag(struct packet *, struct netaddr *);

#endif                          /* _IPv4_H_ */
