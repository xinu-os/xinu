/*
 * @file snoop.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _SNOOP_H_
#define _SNOOP_H_

#include <stddef.h>
#include <arp.h>
#include <ethernet.h>
#include <ipv4.h>
#include <mailbox.h>
#include <network.h>
#include <tcp.h>
#include <udp.h>

/* Tracing macros */
//#define TRACE_SNOOP     TTY1
#ifdef TRACE_SNOOP
#include <stdio.h>
#define SNOOP_TRACE(...)     { \
	fprintf(TRACE_SNOOP, "%s:%d (%d) ", __FILE__, __LINE__, gettid()); \
    fprintf(TRACE_SNOOP, __VA_ARGS__); \
	fprintf(TRACE_SNOOP, "\n"); }
#else
#define SNOOP_TRACE(...)
#endif

/* Dump constants */
#define SNOOP_DUMP_NONE     0
#define SNOOP_DUMP_HEX      1
#define SNOOP_DUMP_CHAR     2

/* Verbose constants */
#define SNOOP_VERBOSE_NONE  0
#define SNOOP_VERBOSE_ONE   1
#define SNOOP_VERBOSE_TWO   2

/* Filter constants */
#define SNOOP_FILTER_ALL	0
#define SNOOP_FILTER_ARP	1
#define SNOOP_FILTER_IPv4   2
#define SNOOP_FILTER_UDP    3
#define SNOOP_FILTER_TCP    4
#define SNOOP_FILTER_ICMP   5

#define SNOOP_QLEN          100

struct snoop
{
    uint caplen;                          /**< bytes of packet to capture   */

    bool promisc;                         /**< promiscous mode enabled      */
    uchar type;                           /**< type of packets to capture   */
    struct netaddr srcaddr;               /**< source address of packets    */
    ushort srcport;                       /**< source port of packets       */
    struct netaddr dstaddr;               /**< destination address of pkts  */
    ushort dstport;                       /**< destination port of packets  */

    mailbox queue;                        /**< mailbox for queueing packets */

    uint ncap;
    uint nmatch;
    uint novrn;
    uint nprint;
};

/* Function prototypes */
int snoopCapture(struct snoop *cap, struct packet *pkt);
int snoopClose(struct snoop *cap);
bool snoopFilter(struct snoop *cap, struct packet *pkt);
int snoopOpen(struct snoop *cap, char *devname);
int snoopPrint(struct packet *pkt, char dump, char verbose);
int snoopPrintArp(struct arpPkt *arp, char verbose);
int snoopPrintEthernet(struct etherPkt *ether, char verbose);
int snoopPrintIpv4(struct ipv4Pkt *ip, char verbose);
int snoopPrintTcp(struct tcpPkt *tcp, char verbose);
int snoopPrintUdp(struct udpPkt *udp, char verbose);
struct packet *snoopRead(struct snoop *cap);

#endif                          /* _SNOOP_H_ */
