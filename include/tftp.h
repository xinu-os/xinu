/**
 * @file tftp.h
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#ifndef _TFTP_H_
#define _TFTP_H_

#include <stddef.h>
#include <network.h>
#include <stdint.h>

#define TFTP_OPCODE_RRQ   1
#define TFTP_OPCODE_WRQ   2
#define TFTP_OPCODE_DATA  3
#define TFTP_OPCODE_ACK   4
#define TFTP_OPCODE_ERROR 5

#define TFTP_RECV_THR_STK   NET_THR_STK
#define TFTP_RECV_THR_PRIO  NET_THR_PRIO

/* Maximum number of seconds to wait for a block, other than the first, before
 * aborting the TFTP transfer.  */
#define TFTP_BLOCK_TIMEOUT      10

/** Maximum number of seconds to wait for the first block before re-sending the
 * RREQ.  */
#define TFTP_INIT_BLOCK_TIMEOUT 1

/** Maximum number of times to send the initial RREQ.  */
#define TFTP_INIT_BLOCK_MAX_RETRIES 10

#define TFTP_BLOCK_SIZE     512

//#define ENABLE_TFTP_TRACE

#ifdef ENABLE_TFTP_TRACE
#  include <stdio.h>
#  include <thread.h>
#  define TFTP_TRACE(format, ...)                                     \
do                                                                    \
{                                                                     \
    fprintf(stderr, "%s:%d (%d) ", __FILE__, __LINE__, gettid());     \
    fprintf(stderr, format, ## __VA_ARGS__);                          \
    fprintf(stderr, "\n");                                            \
} while (0)
#else
#  define TFTP_TRACE(format, ...)
#endif

struct tftpPkt
{
    uint16_t opcode;
    union
    {
        struct
        {
            char filename_and_mode[2 + TFTP_BLOCK_SIZE];
        } RRQ;
        struct
        {
            uint16_t block_number;
            uint8_t data[TFTP_BLOCK_SIZE];
        } DATA;
        struct
        {
            uint16_t block_number;
        } ACK;
    };
};

#define TFTP_MAX_PACKET_LEN      516

/**
 * @ingroup tftp
 *
 * Type of a caller-provided callback function that consumes data downloaded by
 * tftpGet().  See tftpGet() for more details.
 */
typedef int (*tftpRecvDataFunc)(const uchar *data, uint len, void *ctx);

syscall tftpGet(const char *filename, const struct netaddr *local_ip,
                const struct netaddr *server_ip, tftpRecvDataFunc recvDataFunc,
                void *recvDataCtx);

syscall tftpGetIntoBuffer(const char *filename, const struct netaddr *local_ip,
                          const struct netaddr *server_ip, uint *len_ret);

thread tftpRecvPackets(int udpdev, struct tftpPkt *pkt, tid_typ parent);

syscall tftpSendACK(int udpdev, ushort block_number);

syscall tftpSendRRQ(int udpdev, const char *filename);

#endif /* _TFTP_H_ */
