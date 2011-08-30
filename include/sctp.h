/**
 * @file sctp.h
 * 
 * $Id: tcp.h 2134 2009-11-20 02:12:30Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _SCTP_H_
#define _SCTP_H_

#include <conf.h>
#include <stdio.h>
#include <stddef.h>
#include <stdarg.h>
#include <network.h>
#include <mailbox.h>

/* Tracing macros */
#define TRACE_SCTP     TTY1
#ifdef TRACE_SCTP
#define SCTP_TRACE(...)     { \
		fprintf(TRACE_SCTP, "%s:%d (%d) ", __FILE__, __LINE__, gettid()); \
		fprintf(TRACE_SCTP, __VA_ARGS__); \
		fprintf(TRACE_SCTP, "\n"); }
#else
#define SCTP_TRACE(...)
#endif

/**********************
 * DEVICE Definitions *
 **********************/

/**
 * Ensure SCTP device is valid
 * @param dev device id number to verify
 */
#define isbadsctp(dev)  (!(SCTP0 <= (dev) && (dev) < (SCTP0 + NSCTP)))

/* Buffer lengths */
#define SCTP_IBLEN (1<<14) /**< length of input buffer */
#define SCTP_OBLEN (1<<14) /**< length of output buffer */

/* System constants */
#define SCTP_MAX_IPS 4     /**< Max. number of IPs per association */
#define SCTP_MAX_STREAMS 4 /**< Max. number of streams per association */

#define SCTP_DEV_FREE 0    /**< SCTP "device" is free for use */
#define SCTP_DEV_USED 1    /**< SCTP device is in some used state */

struct sctp_stream
{
    int id;
};

struct sctp
{
    /* Xinu-oriented details */
    device *devptr;            /**< device entry associated with this TCB */
    semaphore lock;            /**< Lock to protect this TCB entry */
    int dev_state;             /**< Device state */
    mailbox signal;            /**< Mailbox for signalling packets */
    uint secret;               /**< Secret key */

    /* Instance details. RFC 4960, pg. 129 */
    ushort localport;          /**< Local port number */
    struct netaddr localip[SCTP_MAX_IPS];      /**< Local IPs for association */

    /* Association details. RFC 4960, pg. 129-- */
    uint peer_tag;             /**< peer verification tag */
    uint my_tag;               /**< my verification tag */

    short state;               /**< Connection state */

    ushort remoteport;         /**< Remote port number */
    struct netaddr remoteip[SCTP_MAX_IPS];     /**< Remote IPs for association */

    int primary_path;          /**< Index of remote IP that is "primary" */

    uint err_count;
    uint err_threshold;

    uint peer_rwnd;
    uint next_tsn;
    uint last_tsn;
    /* XXX: mapping array ??? */
    int ack_state;

    struct sctp_stream inboud[SCTP_MAX_STREAMS];
    struct sctp_stream outboud[SCTP_MAX_STREAMS];

    char reasm_queue[1];
    char out_queue[1];
    char in_queue[1];
    uint mtu;

    /* per transport address data */
    struct
    {
        uint err_count;
        uint err_threshold;
        uint cwnd;
        uint ssthresh;
        uint rto;
        uint srtt;
        uint rtt_var;
        uint bytes_acked;
        int state;
        uint pmtu;
        int timer_id;
        uint rto_pending;
        uint last_time;
    } trans[SCTP_MAX_IPS];
};

extern struct sctp sctptab[NSCTP];

/************************
 * PROTOCOL Definitions *
 ************************/

/* SCTP common header format */
struct sctpHeader
{
    ushort srcpt;         /**< source port number */
    ushort dstpt;         /**< destination port number */
    uint tag;             /**< verification tag */
    uint checksum;        /**< checksum */
};

/* SCTP generic chunk format (should be padded to 4 byte boundary) */
struct sctpChunkHeader
{
    uchar type;               /**< type of SCTP chunk */
    uchar flags;              /**< flags for this chunk (type specific) */
    ushort length;            /**< length of this chunk (bytes) */
};

/* Accepted SCTP Chunk Types (as of RFC 4960) */
#define SCTP_TYPE_DATA 0               /**< Payload data */
#define SCTP_TYPE_INIT 1               /**< Initiation */
#define SCTP_TYPE_INIT_ACK 2           /**< Initiation Acknowledgement */
#define SCTP_TYPE_SACK 3               /**< Selective Acknowledgement */
#define SCTP_TYPE_HEARTBEAT 4          /**< Heartbeat Request */
#define SCTP_TYPE_HEARTBEAT_ACK 5      /**< Heartbeat Acknowledgement */
#define SCTP_TYPE_ABORT 6              /**< Abort */
#define SCTP_TYPE_SHUTDOWN 7           /**< Shutdown */
#define SCTP_TYPE_SHUTDOWN_ACK 8       /**< Shutdown Acknowledgement */
#define SCTP_TYPE_ERROR 9              /**< Operation Error */
#define SCTP_TYPE_COOKIE_ECHO 10       /**< State Cookie */
#define SCTP_TYPE_COOKIE_ACK 11        /**< Cookie Acknowledgement */
#define SCTP_TYPE_SHUTDOWN_COMPLETE 14 /**< Shutdown Complete */

/* SCTP packet format */
struct sctpPkt
{
    struct sctpHeader header;     /**< common header */
    struct sctpChunkHeader chunk[1]; /**< one or more chunk(s) */
};

/* SCTP Cookie -- should have enough to "recreate" TCB. RFC 4960, pg. 61 */
struct sctpCookie
{
    uint mac;                /**< Message Authentication Code */
    uint create_time;        /**< Local creation time of TCB */
    uint life_time;          /**< Time at which this Cookie expires */
    struct netaddr remoteip; /**< Source IP of the INIT packet */
    ushort remotept;         /**< Source port of the INIT packet */
};

/**
 * SCTP Chunk Definitions 
 **/
/* SCTP (optional) chunk paramater format */
struct sctpChunkParam
{
    ushort type;              /**< type of SCTP chunk parameter */
    ushort length;            /**< length of this chunk parameter (bytes) */
    uchar value[1];           /**< value of this chunk parameter */
};

/* SCTP DATA Chunk format */
#define SCTP_DATA_FLAG_U 0x04 /**< (U)nordered bit */
#define SCTP_DATA_FLAG_B 0x02 /**< (B)eginning fragment bit */
#define SCTP_DATA_FLAG_E 0x01 /**< (E)nding fragment bit */
struct sctpChunkData
{
    struct sctpChunkHeader head;     /**< chunk header (type, flags, len) */
    uint tsn;                 /**< Transmission Sequence Number */
    ushort stream_id;         /**< Stream Identifier */
    ushort stream_seq;        /**< Stream Sequence Number */
    uint protocol_id;         /**< Payload Protocol Identifier */
    uchar data[1];            /**< User data */
};

/* SCTP INIT Chunk format */
struct sctpChunkInit
{
    struct sctpChunkHeader head;     /**< chunk header (type, flags, len) */
    uint init_tag;            /**< Initiate Tag */
    uint a_rwnd;              /**< Advertised Receiver Window Credit */
    ushort n_out_streams;     /**< Number of Outbound Streams */
    ushort n_in_streams;      /**< Number of Inbound Streams */
    uint init_tsn;            /**< Initial Transmission Sequence Number */
    struct sctpChunkParam param[1];     /**< Optional Parameter list */
};
#define SCTP_INIT_PARAM_IPv4_ADDR       5
#define SCTP_INIT_PARAM_IPv6_ADDR       6
#define SCTP_INIT_PARAM_COOKIE_PRESERVE 9
#define SCTP_INIT_PARAM_ECN_CAPABLE     0x8000
#define SCTP_INIT_PARAM_HOST_NAME       11
#define SCTP_INIT_PARAM_SUPPORTED_ADDR  12

/* SCTP INIT ACK Chunk format */
struct sctpChunkInitAck
{
    struct sctpChunkHeader head;     /**< chunk header (type, flags, len) */
    uint init_tag;            /**< Initiate Tag */
    uint a_rwnd;              /**< Advertised Receiver Window Credit */
    ushort n_out_streams;     /**< Number of Outbound Streams */
    ushort n_in_streams;      /**< Number of Inbound Streams */
    uint init_tsn;            /**< Initial Transmission Sequence Number */
    struct sctpChunkParam param[1];     /**< Optional Parameter list */
};
#define SCTP_INITACK_PARAM_IPv4_ADDR       5
#define SCTP_INITACK_PARAM_IPv6_ADDR       6
#define SCTP_INITACK_PARAM_STATE_COOKIE    7
#define SCTP_INITACK_PARAM_UNRECOGNIZED    8
#define SCTP_INITACK_PARAM_COOKIE_PRESERVE 9
#define SCTP_INITACK_PARAM_ECN_CAPABLE     0x8000
#define SCTP_INITACK_PARAM_HOST_NAME       11

/* SCTP SACK Chunk format */
struct sctpChunkSack
{
    struct sctpChunkHeader head;     /**< chunk header (type, flags, len) */
    uint cum_tsn_ack;         /**< Cumulative TSN Ack */
    uint a_rwnd;              /**< Advertised Receiver Window Credit */
    ushort n_gap_acks;        /**< Number of Gap Ack Blocks */
    ushort n_dup_tsns;        /**< Number of Duplicate TSNs */
    struct
    {
        ushort start;             /**< Gap Ack Block #n start */
        ushort end;               /**< Gap Ack Block #n end */
    } gap_ack[0];
    uint dup_tsn[0];          /**< Duplicate TSN #x */
};

/* SCTP HEARTBEAT (and HEARTBEAT ACK) Chunk format */
struct sctpChunkHeartbeat
{
    struct sctpChunkHeader head;     /**< chunk header (type, flags, len) */
    struct sctpChunkParam info[1];     /**< Heartbeat info */
};
#define SCTP_HEARTBEAT_SENDER_INFO 1

/* SCTP ABORT Chunk format */
struct sctpChunkAbort
{
    struct sctpChunkHeader head;     /**< chunk header (type, flags, len) */
    struct sctpChunkParam err_cause[0];     /**< zero or more error causes */
};
#define SCTP_ABORT_FLAG_T 0x01 /**< Sender filled verification tag set */

/* SCTP SHUTDOWN Chunk format */
struct sctpChunkShutdown
{
    struct sctpChunkHeader head;     /**< chunk header (type, flags, len) */
    uint cum_tsn_ack;         /**< Cumulative TSN Ack */
};

/* SCTP SHUTDOWN ACK Chunk has no parameters */

/* SCTP ERROR Chunk format */
struct sctpChunkError
{
    struct sctpChunkHeader head;     /**< chunk header (type, flags, len) */
    struct sctpChunkParam err_cause[1];     /**< one or more error causes */
};
/* Defined on RFC 4960, pages 43--50 */
#define SCTP_ERR_CAUSE_INVALID_STREAM        1
#define SCTP_ERR_CAUSE_MISSING_PARAM         2
#define SCTP_ERR_CAUSE_STALE_COOKIE          3
#define SCTP_ERR_CAUSE_OUT_OF_RESOURCE       4
#define SCTP_ERR_CAUSE_UNRESOLVE_ADDR        5
#define SCTP_ERR_CAUSE_UNRECOG_CHUNK         6
#define SCTP_ERR_CAUSE_INVALID_PARAM         7
#define SCTP_ERR_CAUSE_UNRECOG_PARAM         8
#define SCTP_ERR_CAUSE_NO_USER_DATA          9
#define SCTP_ERR_CAUSE_COOKIE_WHILE_SHUTDOWN 10
#define SCTP_ERR_CAUSE_RESTART_WITH_NEW_ADDR 11
#define SCTP_ERR_CAUSE_USER_ABORT            12
#define SCTP_ERR_CAUSE_PROTO_VIOLATION       13

/* SCTP COOKIE ECHO Chunk format */
struct sctpChunkCookieEcho
{
    struct sctpChunkHeader head;     /**< chunk header (type, flags, len) */
    uchar cookie[1];     /**< Cookie */
};

/* SCTP COOKIE ACK Chunk has no parameters */

/* SCTP SHUTDOWN COMPLETE Chunk has no parameters */
#define SCTP_SHUTDOWN_COMPLETE_FLAG_T 0x01 /**< verification tag set */

/*****************************
 * STATE MACHINE Definitions *
 *****************************/
#define SCTP_STATE_CLOSED            0
#define SCTP_STATE_COOKIE_WAIT       1
#define SCTP_STATE_COOKIE_ECHOED     2
#define SCTP_STATE_ESTABLISHED       3
#define SCTP_STATE_SHUTDOWN_PENDING  4
#define SCTP_STATE_SHUTDOWN_SENT     5
#define SCTP_STATE_SHUTDOWN_RECEIVED 6
#define SCTP_STATE_SHUTDOWN_ACK_SENT 7

/*********************
 * Timer Definitions *
 *********************/
struct sctpTimer
{
    int id;                       /**< arbitrary identifier for timer */
    uint ms;                      /**< length of timer */
    uint remain_ms;               /**< remaining time on entry */
    void (*callback) (void *);    /**< callback function when timer fires */
    void *args;                   /**< arguments for callback function */
    struct sctpTimer *prev;       /**< previous timer entry pointer */
    struct sctpTimer *next;       /**< next timer entry pointer */
};

extern struct sctpTimer *head;

/*****************************************
 * Definitions that SHOULDN'T BE HERE... *
 *****************************************/
/* Dynamic/private (ephemeral) port allocation range */
#define SCTP_LOWPORT  49152 /**< low port number (IANA suggestion) */
#define SCTP_HIGHPORT 65535 /**< highest port number */

/* Upper Layer Protocol functions */
// TODO: this.

/* Input/Output functions and helpers */
int sctpInput(struct packet *, struct netaddr *, struct netaddr *);
int sctpOutput(struct sctp *, void *, uint);
uint sctpChecksum(void *, uint);

struct sctpTimer *sctpTimerStart(uint, int, void (*callback) (void *),
                                 void *);
int sctpTimerCancel(struct sctpTimer *timer);
struct sctpTimer *sctpTimerMatch(int id);
thread sctpTimerThread(void);

/* Function prototypes */
devcall sctpInit(device *);
devcall sctpOpen(device *, va_list);
devcall sctpClose(device *);
devcall sctpGetc(device *);
devcall sctpRead(device *, void *, uint);
devcall sctpPutc(device *, char);
devcall sctpWrite(device *, void *, uint);
devcall sctpControl(device *, int, long, long);

#endif                          /* _SCTP_H_ */
