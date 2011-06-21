/**
 * @file udp.h
 *
 * $Id: udp.h 2114 2009-11-03 01:13:43Z zlund $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _UDP_H_
#define _UDP_H_

#include <stddef.h>
#include <network.h>
#include <ipv4.h>
#include <semaphore.h>
#include <stdarg.h>

/* Tracing macros */
//#define TRACE_UDP     TTY1
#ifdef TRACE_UDP
#include <stdio.h>
#define UDP_TRACE(...)     { \
        fprintf(TRACE_UDP, "%s:%d (%d) ", __FILE__, __LINE__, gettid()); \
        fprintf(TRACE_UDP, __VA_ARGS__); \
        fprintf(TRACE_UDP, "\n"); }
#else
#define UDP_TRACE(...)
#endif

/* UDP definitions */
#define UDP_HDR_LEN	        8
#define UDP_PSEUDO_LEN      12
#define UDP_MAX_PKTS        100
#define UDP_MAX_DATALEN     1024
#define UDP_TTL             64

/* UDP standard ports */
#define UDP_PORT_RDATE  	37
#define UDP_PORT_DNS		53
#define UDP_PORT_DHCPS  	67
#define UDP_PORT_DHCPC  	68
#define UDP_PORT_TRACEROUTE	33434

/* UDP flags */
#define UDP_FLAG_INCHDR     0x01
#define UDP_FLAG_NOBLOCK    0x02
#define UDP_FLAG_BINDFIRST  0x04

/* UDP state constants */
#define UDP_FREE      0
#define UDP_ALLOC     1
#define UDP_OPEN      2

/* UDP control functions */
#define UDP_CTRL_ACCEPT     1   /**< Set the local port and ip address  */
#define UDP_CTRL_BIND       2   /**< Set the remote port and ip address */
#define UDP_CTRL_CLRFLAG    3   /**< Clear flag(s)                      */
#define UDP_CTRL_SETFLAG    4   /**< Set flag(s)                        */

/* Local port allocation ranges */
#define UDP_PSTART  10000   /**< start port for allocating */
#define UDP_PMAX    65000   /**< max UDP port */

#ifndef __ASSEMBLER__

/*
 * UDP HEADER
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Ethernet Header (14 octets)                                   |
 * | ...                                                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | IP Header (20 octets)                                         |
 * | ...                                                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Source Port                   | Destination Port              |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Message Length                | Checksum                      |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Data (Variable octets)                                        |
 * | ...                                                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

struct udpPkt                   /* UDP Packet Variables */
{
    ushort srcPort;             /**< UDP Source port                */
    ushort dstPort;             /**< UDP Destination port           */
    ushort len;                 /**< UDP length, includes header    */
    ushort chksum;              /**< UDP Checksum                   */
    uchar data[1];              /**< UDP data                       */
};

/*
 * UDP PSEUDO HEADER (for computing checksum)
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Source IP Address                                             |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Destination IP Address                                        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Zero          | Protocol        | UDP Length                  |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */

struct udpPseudoHdr             /* Used for checksum */
{
    uchar srcIp[IPv4_ADDR_LEN];     /**< Source IP address      */
    uchar dstIp[IPv4_ADDR_LEN];     /**< Destination IP address */
    uchar zero;                     /**< Field of all zeroes    */
    uchar proto;                    /**< Protocol used (UDP=17) */
    ushort len;                     /**< Length of UDP packet   */
};

/* UDP Control Block */

struct udp
{
    device *dev;                        /**< UDP device entry               */
    struct udpPkt *in[UDP_MAX_PKTS];    /**< Pointers to stored packets     */
    int inPool;                         /**< Pool of received UDP packets   */
    int icount;                         /**< Count value for input buffer   */
    int istart;                         /**< Start value for input buffer   */
    semaphore isem;                     /**< Semaphore for input buffer     */

    ushort localpt;                     /**< UDP local port                 */
    ushort remotept;                    /**< UDP remote port                */
    struct netaddr localip;             /**< UDP local IP address           */
    struct netaddr remoteip;            /**< UDP remote IP address          */

    uchar state;                        /**< UDP state                      */
    uchar flags;                        /**< UDP flags                      */
};

extern struct udp udptab[];

/* Function Prototypes */
devcall udpInit(device *);
devcall udpOpen(device *, va_list);
devcall udpClose(device *);
devcall udpRead(device *, void *, uint);
devcall udpWrite(device *, void *, uint);
ushort udpAlloc(void);
ushort udpChksum(struct packet *, ushort, struct netaddr *,
                 struct netaddr *);
struct udp *udpDemux(ushort, ushort, struct netaddr *, struct netaddr *);
syscall udpRecv(struct packet *, struct netaddr *, struct netaddr *);
syscall udpSend(struct udp *, ushort, void *);
devcall udpControl(device *, int, long, long);
struct udpPkt *udpGetbuf(struct udp *);
syscall udpFreebuf(struct udpPkt *);

#endif                          /* __ASSEMBLER__ */

#endif                          /* _UDP_H_ */
