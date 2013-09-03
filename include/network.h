/*
 * @file network.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <stddef.h>
#include <conf.h>
#include <ethernet.h>
#include <string.h>

/** @ingroup network
 *  @{
 */

/* Tracing macros */
//#define TRACE_NET     TTY1
#ifdef TRACE_NET
#include <stdio.h>
#define NET_TRACE(...)     { \
		fprintf(TRACE_NET, "%s:%d (%d) ", __FILE__, __LINE__, gettid()); \
		fprintf(TRACE_NET, __VA_ARGS__); \
		fprintf(TRACE_NET, "\n"); }
#else
#define NET_TRACE(...)
#endif

/* Endian conversion macros*/
#if BYTE_ORDER == LITTLE_ENDIAN
#define hs2net(x) (unsigned) ((((x)>>8) &0xff) | (((x) & 0xff)<<8))
#define net2hs(x) hs2net(x)
#define hl2net(x)   ((((x)& 0xff)<<24) | (((x)>>24) & 0xff) | \
    (((x) & 0xff0000)>>8) | (((x) & 0xff00)<<8))
#define net2hl(x) hl2net(x)
#else
#define hs2net(x) (x)
#define net2hs(x) (x)
#define hl2net(x) (x)
#define net2hl(x) (x)
#endif

/* Maximum network address length */
#define NET_MAX_ALEN    6

/**
 * @ingroup network
 * Network address
 */
struct netaddr
{
    ushort type;                      /**< Address type (NETADDR_*)     */
    uchar len;                        /**< Address length               */
    uchar addr[NET_MAX_ALEN];         /**< Address                      */
};

/* Netwok address types */

/** @ingroup network */
#define NETADDR_ETHERNET    1
/** @ingroup network */
#define NETADDR_IPv4        ETHER_TYPE_IPv4

extern const struct netaddr NETADDR_GLOBAL_IP_BRC;
extern const struct netaddr NETADDR_GLOBAL_ETH_BRC;

/* Network address macros */
bool netaddrequal(const struct netaddr *, const struct netaddr *);
syscall netaddrmask(struct netaddr *, const struct netaddr *);
syscall netaddrhost(struct netaddr *, const struct netaddr *);
/** @ingroup network */
#define netaddrcpy(dst, src)     memcpy(dst, src, sizeof(struct netaddr))
int netaddrsprintf(char *, const struct netaddr *);

/* Standard underlying network device driver control functions */

#define NET_GET_MTU         200
#define NET_GET_LINKHDRLEN  201
#define NET_GET_HWADDR      203
#define NET_GET_HWBRC       204

/* Network interface structure definitions */
#ifdef NETHER
#ifdef NETHLOOP
#define NNETIF    NETHER+NETHLOOP     /**< Num network interaces        */
#else
#define NNETIF    NETHER              /**< Num network interaces        */
#endif
#endif


/* Network receive thread constants */
#define NET_NTHR       5              /**< Num net receive threads      */
#define NET_THR_PRIO   30             /**< Net recv thread priority     */
#define NET_THR_STK    4096           /**< Net recv thread stack size   */

/* Network table entry states */
#define NET_FREE   0                  /**< Netif state free             */
#define NET_ALLOC  1                  /**< Netif state allocated        */

/** Net interface control block */
struct netif
{
    int dev;                          /**< underlying device struct     */
    ushort state;                     /**< Table entry state            */
    uint mtu;                         /**< MTU for network device       */
    uint linkhdrlen;                  /**< Length of link layer header  */
    struct netaddr ip;                /**< Protocol addr for interface  */
    struct netaddr mask;              /**< Protocol address subnet mask */
    struct netaddr gateway;           /**< Gateway protocol address     */
    struct netaddr ipbrc;             /**< Broadcast protocol address   */
    struct netaddr hwaddr;            /**< Hardware address             */
    struct netaddr hwbrc;             /**< Hardware broadcast address   */
    tid_typ recvthr[NET_NTHR];        /**< Recv thread ids              */
    uint nin;                         /**< Num recv pkts                */
    uint nproc;                       /**< Num recv pkts processed      */
    void *capture;                    /**< Snoop capture structure      */
};

extern struct netif netiftab[];

/** Network packet buffer pool */
extern int netpool;

#define NET_MAX_PKTLEN		1598    /**< Mod 4 of this constant must be 2 */
/**
 * Poolsize should be >= ARP_NQUEUE + RT_NQUEUE + UDP_IBLEN + RAW_IBLEN 
 */
#define NET_POOLSIZE		512

/** Incoming packet structure       */
struct packet
{
    struct netif *nif;          /**< Interface for packet               */
    uint len;                   /**< Total Length of the packet         */
    uchar *linkhdr;             /**< Pointer to link layer header       */
    uchar *nethdr;              /**< Pointer to network layer header    */
    uchar *curr;                /**< Pointer to location into packet    */
    uchar pad[2];               /**< Padding for word alignment         */
    uchar data[1];              /**< Pointer to incoming packet         */
};

/* Function Prototypes */
ushort netChksum(void *, uint);
syscall netDown(int);
syscall netFreebuf(struct packet *);
struct packet *netGetbuf(void);
syscall netInit(void);
struct netif *netLookup(int);
thread netRecv(struct netif *);
syscall netSend(struct packet *, const struct netaddr *, const struct netaddr *,
                ushort);
syscall netUp(int, const struct netaddr *, const struct netaddr *,
              const struct netaddr *);

/**  @} */

#endif                          /* _NETWORK_H_ */
