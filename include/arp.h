/**
 * @file arp.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _ARP_H_
#define _ARP_H_

#include <stddef.h>
#include <mailbox.h>
#include <network.h>

/* Tracing macros */
//#define TRACE_ARP     TTY1
#ifdef TRACE_ARP
#include <stdio.h>
#define ARP_TRACE(...)     { \
		fprintf(TRACE_ARP, "%s:%d (%d) ", __FILE__, __LINE__, gettid()); \
		fprintf(TRACE_ARP, __VA_ARGS__); \
		fprintf(TRACE_ARP, "\n"); }
#else
#define ARP_TRACE(...)
#endif

/* ARP Hardware Types */
#define ARP_HWTYPE_ETHERNET   1            /**< Hardware type ethernet */

/* ARP Protocol Types */
#define ARP_PRTYPE_IPv4       ETHER_TYPE_IPv4  /**< Protocol type IPv4     */

/* ARP Operations */
#define ARP_OP_RQST 1              /**< ARP operation request           */
#define ARP_OP_REPLY 2             /**< ARP operation reply             */

/* ARP Header */
#define ARP_CONST_HDR_LEN   8

/* ARP Table */
#define ARP_NENTRY         32      /**< Number of ARP table entries     */
#define ARP_FREE           0       /**< Entry is free                   */
#define ARP_USED           1       /**< Entry is used                   */
/* ARP entry is unresolved if it is USED but not RESOLVED (0b01) */
#define ARP_UNRESOLVED     1       /**< Entry is used but unresolved    */
/* ARP entry is resolved if it is USED and RESOLVED (0b11) */
#define ARP_RESOLVED        3      /**< Entry is used and resolved      */
#define ARP_NTHRWAIT        10     /**< Num threads that can wait       */

/* ARP Lookup */
#define ARP_MAX_LOOKUP      5     /**< Num ARP lookup attempts per pkt  */
#define ARP_MSG_RESOLVED    1     /**< Message shows arp resolution     */

/* Timing info */
#define ARP_TTL_UNRESOLVED  5    /**< TTL in secs for unresolv entry  */
#define ARP_TTL_RESOLVED    300   /**< TTL in secs for resolved entry  */

/* ARP thread constants */
#define ARP_THR_PRIO        NET_THR_PRIO   /**< ARP thread priority     */
#define ARP_THR_STK         NET_THR_STK    /**< ARP thread stack size   */

/* ARP packet header offset macros */
#define ARP_ADDR_SHA(arp)   0
#define ARP_ADDR_SPA(arp)   (arp->hwalen)
#define ARP_ADDR_DHA(arp)   (arp->hwalen + arp->pralen)
#define ARP_ADDR_DPA(arp)   ((2 * arp->hwalen) + arp->pralen)

/* ARP daemon info */
#define ARP_NQUEUE          32    /**< Number of pkts allowed in queue  */

/*
 * ARP HEADER
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Link-Level Header                                             |
 * | ...                                                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |        Hardware Type          |         Protocol Type         |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Hardware Len | Protocol Len   |          Operation            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Source Hardware Address (SHA)                                 |
 * | Source Protocol Address (SPA)                                 |
 * | Destination Hardware Address (DHA)                            |
 * | Destination Protocol Address (DPA)                            |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
/* ARP packet header */
struct arpPkt
{
    ushort hwtype;              /**< Hardware type                   */
    ushort prtype;              /**< Protocol type                   */
    uchar hwalen;               /**< Hardware address length         */
    uchar pralen;               /**< Protocol address length         */
    ushort op;                  /**< Operation                       */
    uchar addrs[1];                 /**< Src & dst hdwr & proto addrs    */
};

/* ARP Table Entry */
struct arpEntry
{
    ushort state;                    /**< ARP state for entry           */
    struct netif *nif;                   /**< Network interface             */
    struct netaddr hwaddr;               /**< Hardware address              */
    struct netaddr praddr;               /**< Protocol address              */
    uint expires;                    /**< clktime when entry expires    */
    tid_typ waiting[ARP_NTHRWAIT];   /**< Threads waiting for entry     */
    int count;                       /**< Count of threads waiting      */
};

/* ARP table */
extern struct arpEntry arptab[ARP_NENTRY];

/* ARP packet queue for packets requiring reply */
extern mailbox arpqueue;

/* ARP Function Prototypes */
struct arpEntry *arpAlloc(void);
thread arpDaemon(void);
struct arpEntry *arpGetEntry(const struct netaddr *);
syscall arpFree(struct arpEntry *);
syscall arpInit(void);
syscall arpLookup(struct netif *, const struct netaddr *, struct netaddr *);
syscall arpNotify(struct arpEntry *, message);
syscall arpRecv(struct packet *);
syscall arpSendRqst(struct arpEntry *);
syscall arpSendReply(struct packet *);

#endif                          /* _ARP_H_ */
