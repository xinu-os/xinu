/**
 * @file route.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _ROUTE_H_
#define _ROUTE_H_

#include <stddef.h>
#include <mailbox.h>
#include <network.h>

/* Tracing macros */
//#define TRACE_RT     TTY1
#ifdef TRACE_RT
#include <stdio.h>
#define RT_TRACE(...)     { \
		fprintf(TRACE_RT, "%s:%d (%d) ", __FILE__, __LINE__, gettid()); \
		fprintf(TRACE_RT, __VA_ARGS__); \
		fprintf(TRACE_RT, "\n"); }
#else
#define RT_TRACE(...)
#endif

/* Route Table (Must include at least one entry for default route) */
#define RT_NENTRY         32       /**< Number of ARP table entries     */
#define RT_FREE           0        /**< Entry is free                   */
#define RT_USED           1        /**< Entry is used                   */
#define RT_PEND           2        /**< Entry is pending                */

/* Route thread constants */
#define RT_THR_PRIO        NET_THR_PRIO   /**< Route thread priority    */
#define RT_THR_STK         NET_THR_STK    /**< Route thread stack size  */

/* Route daemon info */
#define RT_NQUEUE          32      /**< Number of pkts allowed in queue */

/* Route Packet Structure */
struct rtEntry
{
    ushort state;
    ushort masklen;
    struct netaddr dst;
    struct netaddr gateway;
    struct netaddr mask;
    struct netif *nif;
};

/* Route table */
extern struct rtEntry rttab[RT_NENTRY];

/* Route pakcet queue for packets requiring routing */
extern mailbox rtqueue;

/* Function prototypes */
syscall rtAdd(const struct netaddr *dst, const struct netaddr *gate,
              const struct netaddr *mask, struct netif *nif);
struct rtEntry *rtAlloc(void);
thread rtDaemon(void);
syscall rtDefault(const struct netaddr *gate, struct netif *nif);
syscall rtInit(void);
struct rtEntry *rtLookup(const struct netaddr *addr);
syscall rtRecv(struct packet *pkt);
syscall rtRemove(const struct netaddr *dst);
syscall rtClear(struct netif *nif);
syscall rtSend(struct packet *pkt);

#endif                          /* _ROUTE_H_ */
