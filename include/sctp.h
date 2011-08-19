/**
 * @file sctp.h
 * 
 * $Id: tcp.h 2134 2009-11-20 02:12:30Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _SCTP_H_
#define _SCTP_H_

#include <stddef.h>
#include <conf.h>
#include <ethernet.h>
#include <ipv4.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdio.h>
#include <thread.h>

/* Tracing macros */
//#define TRACE_SCTP     TTY1
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

struct sctp
{
	/* Xinu-oriented details */
	device *devptr;        /**< device entry associated with this TCB */
	semaphore lock;        /**< Lock to protect this TCB entry */
	short dev_state;       /**< Device state */
	short state;           /**< Connection state */

	/* Connection details */
	ushort localport;      /**< Local port number */
	ushort remoteport;     /**< Remote port number */
	struct netaddr localip[SCTP_MAX_IPS];  /**< Local IPs for association */
	struct netaddr remoteip[SCTP_MAX_IPS]; /**< Remote IPs for association */
};

/************************
 * PROTOCOL Definitions *
 ************************/

/* SCTP common header format */
struct sctpHeader
{
    ushort srcpt;         /**< source port number */
    ushort dstpt;         /**< destination port number */
    uint   tag;           /**< verification tag */
    uint   chksum;        /**< checksum */
};

/* SCTP generic chunk format (should be padded to 4 byte boundary) */
struct sctpChunk
{
	uchar  type;          /**< type of SCTP chunk */
	uchar  flags;         /**< flags for this chunk (type specific) */
	ushort length;        /**< length of this chunk (bytes) */
	uchar  value[1];      /**< value of this chunk */
};

/* Accepted SCTP Chunk Types (as of RFC 4960) */
typedef enum {
	data = 0,             /**< Payload data */
	init = 1,             /**< Initiation */
	init_ack = 2,         /**< Initiation Acknowledgement */
	sack = 3,             /**< Selective Acknowledgement */
	heartbeat = 4,        /**< Heartbeat Request */
	heartbeat_ack = 5,    /**< Heartbeat Acknowledgement */
	abort = 6,            /**< Abort */
	shutdown = 7,         /**< Shutdown */
	shutdown_ack = 8,    /**< Shutdown Acknowledgement */
	error = 9,            /**< Operation Error */
	cookie_echo = 10,     /**< State Cookie */
	cookie_ack = 11,      /**< Cookie Acknowledgement */
	shutdown_complete = 14, /**< Shutdown Complete */
} sctpChunkType;

/* SCTP packet format */
struct sctpPkt
{
	struct sctpHeader header; /**< common header */
    struct sctpChunk  chunk[1]; /**< one or more chunk(s) */
};

/**
 * SCTP Chunk Definitions 
 **/
/* SCTP (optional) chunk paramater format */
struct sctpChunkParam
{
	ushort type;          /**< type of SCTP chunk parameter */
	ushort length;        /**< length of this chunk parameter (bytes) */
	uchar  value[1];      /**< value of this chunk parameter */
};

/* SCTP DATA Chunk format */
#define SCTP_DATA_FLAG_U 0x04 /**< (U)nordered bit */
#define SCTP_DATA_FLAG_B 0x02 /**< (B)eginning fragment bit */
#define SCTP_DATA_FLAG_E 0x01 /**< (E)nding fragment bit */
struct sctpChunkData
{
	uint tsn;             /**< Transmission Sequence Number */
	ushort stream_id;     /**< Stream Identifier */
	ushort stream_seq;    /**< Stream Sequence Number */
	uint protocol_id;     /**< Payload Protocol Identifier */
	uchar data[1];        /**< User data */
};

/* SCTP INIT Chunk format */
struct sctpChunkInit
{
	uint init_tag;        /**< Initiate Tag */
	uint a_rwnd;          /**< Advertised Receiver Window Credit */
	ushort n_out_streams; /**< Number of Outbound Streams */
	ushort n_in_streams;  /**< Number of Inbound Streams */
	uint init_tsn;        /**< Initial Transmission Sequence Number */
	struct sctpChunkParam param[1]; /**< Optional Parameter list */
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
	uint init_tag;        /**< Initiate Tag */
	uint a_rwnd;          /**< Advertised Receiver Window Credit */
	ushort n_out_streams; /**< Number of Outbound Streams */
	ushort n_in_streams;  /**< Number of Inbound Streams */
	uint init_tsn;        /**< Initial Transmission Sequence Number */
	struct sctpChunkParam param[1]; /**< Optional Parameter list */
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
	uint cum_tsn_ack;     /**< Cumulative TSN Ack */
	uint a_rwnd;          /**< Advertised Receiver Window Credit */
	ushort n_gap_acks;    /**< Number of Gap Ack Blocks */
	ushort n_dup_tsns;    /**< Number of Duplicate TSNs */
	struct {
		ushort start;     /**< Gap Ack Block #n start */
		ushort end;       /**< Gap Ack Block #n end */
	} gap_ack[0];
	uint dup_tsn[0];      /**< Duplicate TSN #x */
};

/* SCTP HEARTBEAT (and HEARTBEAT ACK) Chunk format */
struct sctpChunkHeartbeat
{
	struct sctpChunkParam info[1]; /**< Heartbeat info */
};
#define SCTP_HEARTBEAT_SENDER_INFO 1

/* SCTP ABORT Chunk format */
struct sctpChunkAbort
{
	struct sctpChunkParam err_cause[0]; /**< zero or more error causes */
};
#define SCTP_ABORT_FLAG_T 0x01 /**< Sender filled verification tag set */

/* SCTP SHUTDOWN Chunk format */
struct sctpChunkShutdown
{
	uint cum_tsn_ack;     /**< Cumulative TSN Ack */
};

/* SCTP SHUTDOWN ACK Chunk has no parameters */

/* SCTP ERROR Chunk format */
struct sctpChunkError
{
	struct sctpChunkParam err_cause[1]; /**< one or more error causes */
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
	uchar cookie[1]; /**< Cookie */
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

/*****************************************
 * Definitions that SHOULDN'T BE HERE... *
 *****************************************/
/* Dynamic/private (ephemeral) port allocation range */
#define SCTP_LOWPORT  49152 /**< low port number (IANA suggestion) */
#define SCTP_HIGHPORT 65535 /**< highest port number */

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
