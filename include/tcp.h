/**
 * @file tcp.h
 * 
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _TCP_H_
#define _TCP_H_

#include <stddef.h>
#include <conf.h>
#include <ethernet.h>
#include <ipv4.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdio.h>
#include <thread.h>

/* Tracing macros */
//#define TRACE_TCP     TTY1
#ifdef TRACE_TCP
#define TCP_TRACE(...)     { \
		fprintf(TRACE_TCP, "%s:%d (%d) ", __FILE__, __LINE__, gettid()); \
		fprintf(TRACE_TCP, __VA_ARGS__); \
		fprintf(TRACE_TCP, "\n"); }
#else
#define TCP_TRACE(...)
#endif

#define TCP_HDR_LEN    20
#define offset2octets(x)  (((x) >> 4) * 4)
#define octets2offset(x)  (((x)/4) <<  4)

#define TCP_MAXSEQ     0xFFFFFFFF

/* TCP device states */
#define TCP_FREE     0
#define TCP_ALLOC    1

/**
 * Check that the tcp device id is between TCP0 and TCP0+NTCP
 *
 * @param f device id number to verify
 */
#define isbadtcp(f)  ( !(TCP0 <= (f) && (f) < (TCP0 + NTCP)) )

typedef unsigned int tcpseq; /**< TCP sequence number */

/* TCP Packet */
struct tcpPkt
{
    ushort srcpt;      /**< source port number */
    ushort dstpt;      /**< destination port number */
    tcpseq seqnum;     /**< sequence num of 1st octet */
    tcpseq acknum;     /**< next seq num sender expects */
    uchar offset;      /**< num words in TCP header */
    uchar control;     /**< control bits */
    ushort window;     /**< sender window size */
    ushort chksum;     /**< checksum of header and data */
    ushort urgent;     /**< urgent pointer offset */
    uchar data[1];     /**< (options and) data */
};

/* TCP Packet Control Flags */
#define TCP_CTRL_URG 0x20 /**< urgent field significant */
#define TCP_CTRL_ACK 0x10 /**< ack field significant */
#define TCP_CTRL_PSH 0x08 /**< push function */
#define TCP_CTRL_RST 0x04 /**< reset connection */
#define TCP_CTRL_SYN 0x02 /**< synchronize seq num */
#define TCP_CTRL_FIN 0x01 /**< no more data from sender */

/* TCP Options */
#define TCP_OPT_END      0 /**< end of option list */
#define TCP_OPT_NOP      1 /**< no operation */
#define TCP_OPT_MSS      2 /**< maximum segment size */
#define TCP_OPT_MSS_SIZE 6 /**< bytes needed for MSS option */
#define TCP_OPT_MSS_LEN  4 /**< length of MSS option */

/* TCP Checksum Pseudo Header */
struct tcpPseudo
{
    uchar srcIp[IPv4_ADDR_LEN];
    uchar dstIp[IPv4_ADDR_LEN];
    uchar zero;
    uchar proto;
    ushort len;
};

#define TCP_PSEUDO_LEN  12

/* Buffer lengths */
#define TCP_IBLEN 16384  /**< Size of input buffer, must be multiple of 8 */
#define TCP_OBLEN 16384  /**< Size of output buffer */

/* Initial sizes */
#define TCP_INIT_MSS (1440 + TCP_HDR_LEN)
//#define TCP_INIT_MSS (4 + TCP_HDR_LEN) 
#define TCP_INIT_WND TCP_INIT_MSS
#define TCP_MAX_WND 65535

/**
 * Transmission control block 
 */
struct tcb
{
    ushort dev;          /**< TCP device entry */
    uchar state;         /**< connection state */
    uchar devstate;      /**< allocation state of the device internally */
    semaphore mutex;     /**< Mutual exclusion sempahore */

    /* Connection details */
    ushort localpt;             /**< Local port number */
    ushort remotept;            /**< Remote port number */
    struct netaddr localip;     /**< Local IP address */
    struct netaddr remoteip;    /**< Remote IP address */
    uchar opentype;             /**< Type of open call */
    semaphore openclose;

    /* Receive variables */
    tcpseq rcvnxt;              /**< receive next */
    tcpseq rcvwnd;              /**< sequence num for end of receive window */
    tcpseq rcvup;               /**< receive urgent pointer */
    tcpseq rcvfin;              /**< sequence number for received FIN */
    ushort rcvmss;              /**< maximum receive segment size */
    uchar rcvflg;               /**< receive flags */

    /* Receive buffer */
    semaphore readers;          /**< Count of readers waiting for data */
    uint istart;                /**< Index of first octet ready for user */
    uint icount;                /**< Count of octets ready for user */
    uint inxt;
    uchar in[TCP_IBLEN];        /**< Input buffer */
    uchar imark[TCP_IBLEN];
    uint ibytes;                /**< Count of bytes passed to user */

    /* Send variables */
    tcpseq snduna;                  /**< send unacknowledged */
    tcpseq sndnxt;                  /**< send next */
    uint sndwnd;                    /**< send window */
    uint sndcwn;                    /**< send congestion window */
    uint sndsst;                    /**< send slow start threshold */
    tcpseq sndup;                   /**< send urgent pointer */
    tcpseq sndwl1;                  /**< seq num for last win update */
    tcpseq sndwl2;                  /**< ack num for last win update */
    tcpseq iss;                     /**< initial send seq num */
    tcpseq sndfin;                  /**< sequence number for sent FIN */
    ushort sndmss;                  /**< maximum send segment size */
    uchar sndflg;                   /**< send flags */
    int sndrtt;                     /**< smoothed sending round trip time */
    int sndrtd;                     /**< sending round trip deviation */
    int rxttime;                    /**< retransmission timer */
    uint rxtcount;                  /**< number of retransmissions */
    int psttime;                    /**< persist timer */

    /* Send buffer */
    semaphore writers;         /**< Count of writers waiting for buffer */
    uint ostart;               /**< Index of first octet */
    uint ocount;               /**< Octets in buffer */
    uchar out[TCP_OBLEN];      /**< Output buffer */
    uint obytes;               /**< Count of bytes acknowledged by receiver */
};

extern struct tcb tcptab[];

/* Local port allocation ranges */
#define TCP_PSTART 10000     /**< start port for allocating */
#define TCP_PMAX   65000        /**< max TCP port */

/* Open type */
#define TCP_PASSIVE  0  /**< passive open connection */
#define TCP_ACTIVE   1  /**< active open connection */

/* Connection states */
#define TCP_CLOSED   0 /**< closed state */
#define TCP_LISTEN   1 /**< listen state */
#define TCP_SYNSENT  2 /**< syn-sent state */
#define TCP_SYNRECV  3 /**< syn-recevied state */
#define TCP_ESTAB    4 /**< established state */
#define TCP_FINWT1   5 /**< fin-wait-1 state */
#define TCP_FINWT2   6 /**< fin-wait-2 state */
#define TCP_CLOSEWT  7 /**< close-wait state */
#define TCP_CLOSING  8 /**< closing state */
#define TCP_LASTACK  9 /**< last-ack state */
#define TCP_TIMEWT  10 /**< time-wait state */

/* Extra state information */
#define TCP_FLG_SYN      0x01   /**< SYN has been sent/received */
#define TCP_FLG_FIN      0x02   /**< FIN has been sent/received */
#define TCP_FLG_SNDACK   0x04   /**< Need to send an ACK */
#define TCP_FLG_SNDDATA  0x08   /**< Need to send data */
#define TCP_FLG_SNDRST   0x10   /**< Need to send a RST */
#define TCP_FLG_PERSIST  0x20   /**< In persist output state */

#define TCP_SEQINCR 904 /**< amount to increment ISS each time */

/* TCP Errors */
#define TCP_ERR_RESET   -2  /**< connection reset */
#define TCP_ERR_REFUSE  -3  /**< connection refused */
#define TCP_ERR_CLOSING -4  /**< connection closing */
#define TCP_ERR_NOCONN  -5  /**< no connection exists */
#define TCP_ERR_NOSPEC  -6  /**< foreign socket not specified */
#define TCP_ERR_MAXRXT  -7  /**< max num of retransmissions exceeded */
#define TCP_ERR_READTO  -8  /**< read call timed out */
#define TCP_ERR_ESTABTO -9  /**< conenction establishment timed out */

/* TCP Sequence Macros */
#define seqlt(a, b) (((int)(a) - (int)(b)) < 0)
#define seqlte(a, b) (((int)(a) - (int)(b)) <= 0)
#define seqadd(a, b) (((a) + (b)) % TCP_MAXSEQ)

/* TCP Length Macros */
#define tcpSeglen(tcppkt, len) (len - offset2octets(tcppkt->offset))

/* TCP Timer Constants */
#define TCP_NEVENTS     (3*NTCP)+1 /**< max number events (incl dummy head) */
#define TCP_EVT_HEAD    0   /**< Head entry */
#define TCP_FREQ        10  /**< milliseconds per timer tick */
#define TCP_EVT_TIMEWT  1   /**< 2MSL time-wait timeout */
#define TCP_EVT_RXT     2   /**< retransmit event */
#define TCP_EVT_PERSIST 3   /**< persist event, for zero window */

/* TCP Timer Durations */
#define TCP_TWOMSL  (5*1000)
#define TCP_PST_INITTIME (3*1000)  /**< initial persist time */
#define TCP_PST_MAXTIME  (64*1000) /**< maximum persist time */

/* TCP Retransmit */
#define TCP_RXT_MAXCOUNT 10         /** maximum number of retransmissions */
#define TCP_RXT_INITTIME (500)  /**< initial retransmission time */
#define TCP_RXT_MINTIME  (100)    /**< minimum retransmission time */
#define TCP_RXT_MAXTIME  (32*1000) /**< maximum retransmission time */

/* TCP Timer Event */
struct tcpEvent
{
    bool used;                      /**< Is timer event record used? */
    int time;                       /**< number of TCP timer ticks for event */
    int remain;                     /**< TCP timer ticks remain */
    uchar type;                     /**< Type of event */
    struct tcb *tcbptr;             /**< TCB for event */
    struct tcpEvent *next;          /**< Next timer event */
};

extern struct tcpEvent tcptimertab[];
extern semaphore tcpmutex;

/* TCP Control Functions */
#define TCP_CTRL_RECVBYTES 2 /**< Get number of bytes recevied */
#define TCP_CTRL_SENTBYTES 3 /**< Get number of bytes sent */

/* TCP Ports */
#define TCP_PORT_TELNET    23
#define TCP_PORT_HTTP      80

/* Function prototypes */
devcall tcpInit(device *);
devcall tcpOpen(device *, va_list);
devcall tcpClose(device *);
devcall tcpRead(device *, void *, uint);
devcall tcpWrite(device *, void *, uint);
devcall tcpGetc(device *);
devcall tcpPutc(device *, char);
devcall tcpControl(device *, int, long, long);

ushort tcpAlloc(void);
ushort tcpChksum(struct packet *, ushort, struct netaddr *,
                 struct netaddr *);
devcall tcpFree(struct tcb *);
int tcpOpenActive(struct tcb *);
void tcpAbort(struct tcb *, int);
int tcpSetup(struct tcb *);

struct tcb *tcpDemux(ushort, ushort, struct netaddr *, struct netaddr *);
int tcpRecv(struct packet *, struct netaddr *, struct netaddr *);
int tcpRecvOpts(struct packet *, struct tcb *);
int tcpRecvListen(struct packet *, struct tcb *, struct netaddr *);
int tcpRecvSynsent(struct packet *, struct tcb *);
int tcpRecvOther(struct packet *, struct tcb *);
int tcpRecvData(struct packet *, struct tcb *);
bool tcpRecvValid(struct packet *, struct tcb *);
int tcpRecvAck(struct packet *, struct tcb *);
int tcpRecvRtt(struct tcb *);

int tcpSend(struct tcb *, uchar, uint, uint, uint, ushort);
ushort tcpSendWindow(struct tcb *);
int tcpSendAck(struct tcb *);
int tcpSendSyn(struct tcb *);
int tcpSendData(struct tcb *);
int tcpSendRxt(struct tcb *);
int tcpSendPersist(struct tcb *);
int tcpSendRst(struct packet *, struct netaddr *, struct netaddr *);

void tcpStat(struct tcb *);

thread tcpTimer(void);
void tcpTimerTrigger(uchar, struct tcb *);
devcall tcpTimerSched(int, struct tcb *, uchar);
devcall tcpTimerPurge(struct tcb *, uchar);
devcall tcpTimerRemain(struct tcb *, uchar);

tcpseq tcpSeqdiff(tcpseq, tcpseq);

#endif                          /* _TCP_H_ */
