/*
 * @file icmp.h
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _ICMP_H_
#define _ICMP_H_

#include <stddef.h>
#include <network.h>
#include <mailbox.h>
#include <route.h>

/* Tracing macros */
//#define TRACE_ICMP     TTY1
#ifdef TRACE_ICMP
#include <stdio.h>
#define ICMP_TRACE(...)     { \
		fprintf(TRACE_ICMP, "%s:%d (%d) ", __FILE__, __LINE__, gettid()); \
		fprintf(TRACE_ICMP, __VA_ARGS__); \
		fprintf(TRACE_ICMP, "\n"); }
#else
#define ICMP_TRACE(...)
#endif

/* ICMP thread constants */
#define ICMP_THR_PRIO       NET_THR_PRIO   /**< ICMP thread priority     */
#define ICMP_THR_STK        NET_THR_STK    /**< ICMP thread stack size   */

/* ICMP daemon info */
#define ICMP_NQUEUE          10    /**< Number of pkts allowed in queue  */


#define ICMP_RQST_LEN       16
#define ICMP_HEADER_LEN		4       //data not included
#define ICMP_DEF_DATALEN    64
#define PING_TIMEOUT        1000
#define ICMP_MAX_PINGS      10000

/* ICMP Error Definitions */

/* Message Types for ICMP */
#define ICMP_ECHOREPLY      0
#define ICMP_UNREACH        3
#define ICMP_SRCQNCH        4
#define ICMP_REDIRECT       5
#define ICMP_ECHO           8
#define ICMP_TIMEEXCD       11
#define ICMP_PARAMPROB      12
#define ICMP_TMSTMP         13
#define ICMP_TMSTMPREPLY    14
#define ICMP_INFORQST       15
#define ICMP_INFOREPLY      16
#define ICMP_TRACEROUTE     30

/* Message Codes Based Message Types */

//No alternate codes for Echo Reply

//Destination Unreachable Codes
#define ICMP_NET_UNR	0
#define ICMP_HST_UNR	1
#define ICMP_PROTO_UNR	2
#define ICMP_PORT_UNR	3
#define ICMP_FOFF_DFSET	4
#define ICMP_SRCRT_FAIL	5

//No alternate codes for Source Quench

//Redirect Message Codes
#define ICMP_RNET		0
#define ICMP_RHST		1
#define ICMP_RTOS_NET	2
#define ICMP_RTOS_HST	3

//No alternate codes for Echo Request

//Time Exceeded Codes
#define ICMP_TTL_EXC	0
#define ICMP_FRA_EXC	1

//Parameter Problem Codes
#define ICMP_SEE_PNTR		0
#define ICMP_MISSING_OPT	1
#define ICMP_BAD_LENGTH		2

//No alternate codes for Timestamp

//No alternate codes for Timestamp Reply

//No alternate codes for Information Request

//No alternate codes for Information Reply

// Traceroute codes
#define OP_FORWARD	0
#define OP_FAILED	1

//Default Traceroute ttl
#define TRACE_DEFAULT_TTL 30

/*
 * ICMP HEADER
 *
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Link-Level Header                                             |
 * | ...                                                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | IP Header                                                     |
 * | ...                                                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Type          | Code          | Checksum                      |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | Data (Variable octets                                         |
 * | ...                                                           |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */


struct icmpPkt                  /**< ICMP Packet         */
{
    uchar type;                 /**< ICMP type            */
    uchar code;                 /**< ICMP code            */
    ushort chksum;              /**< ICMP checksum        */
    uchar data[1];              /**< ICMP data            */
};

struct icmpEcho
{
    ushort id;
    ushort seq;
    ulong timesec;                  /**< Clock time in seconds, */
    ulong timetic;                  /**< Clock time in tics,    */
    ulong timecyc;                  /**< Clock time in cycles   */
    ulong arrivsec;
    ulong arrivtic;
    ulong arrivcyc;
};

#define NPINGQUEUE 5
#define NPINGHOLD  10

struct icmpEchoQueue
{
    tid_typ tid;        /**< ID of thread that send the ICMP Echo Request.  */
    int head;           /**< Position to store next ICMP Echo Reply.        */
    int tail;           /**< Position of next ICMP Echo Reply to get.       */
    struct packet *pkts[NPINGHOLD]; /*< Stored ICMP Echo Replies.           */
};

extern struct icmpEchoQueue echotab[NPINGQUEUE];

/* ICMP echo request queue */
extern mailbox icmpqueue;

/* ICMP Function Prototypes */
thread icmpDaemon(void);
syscall icmpInit(void);
syscall icmpRecv(struct packet *);
syscall icmpDestUnreach(const struct packet *, uchar);
syscall icmpEchoRequest(struct netaddr *dst, ushort id, ushort seq);
syscall icmpEchoReply(struct packet *request);
syscall icmpRedirect(struct packet *, uchar code, struct rtEntry *);
syscall icmpSend(struct packet *pkt, uchar type, uchar code,
                 uint datalen, struct netaddr *src, struct netaddr *dst);
syscall icmpTimeExceeded(struct packet *, uchar code);

#endif                          /* _NET_H_ */
