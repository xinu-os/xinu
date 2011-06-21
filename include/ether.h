/**
 * @file ether.h
 *
 * $Id: ether.h 2115 2009-11-03 02:31:19Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _ETHER_H_
#define _ETHER_H_

#include <device.h>
#include <ethernet.h>
#include <stdarg.h>
#include <stddef.h>
#include <semaphore.h>

/* Tracing macros */
//#define TRACE_ETHER   TTY1
#ifdef TRACE_ETHER
#include <stdio.h>
#define ETHER_TRACE(...)     { \
        fprintf(TRACE_ETHER, "%s:%d (%d) ", __FILE__, __LINE__, gettid()); \
        fprintf(TRACE_ETHER, __VA_ARGS__); \
        fprintf(TRACE_ETHER, "\n"); }
#else
#define ETHER_TRACE(...)
#endif

#define ETH_ADDR_LEN        6   /**< Length of ethernet address         */

#include <vlan.h>

/* ETH Buffer lengths */
#define ETH_IBLEN           1024 /**< input buffer size                 */

/* Ethernet DMA buffer sizes */
#define ETH_MTU             1500 /**< Maximum transmission units        */
#define ETH_HEADER_LEN      ETH_HDR_LEN  /**< Length of Ethernet header */
#define ETH_VLAN_LEN        4   /**< Length of Ethernet vlan tag        */
#define ETH_CRC_LEN         4   /**< Length of Ethernet CRC             */
#define ETH_MAX_PKT_LEN     ( ETH_HEADER_LEN + ETH_VLAN_LEN + ETH_MTU )

#define ETH_RX_BUF_SIZE     ( ETH_MAX_PKT_LEN + ETH_CRC_LEN \
                              + sizeof(struct rxHeader) )
#define ETH_TX_BUF_SIZE     ( ETH_MAX_PKT_LEN )

/* ETH states */
#define ETH_STATE_FREE       0
#define ETH_STATE_DOWN       1
#define ETH_STATE_UP         2

/* ETH control codes */
#define ETH_CTRL_CLEAR_STATS 1  /**< Clear Ethernet Statistics          */
#define ETH_CTRL_SET_MAC     2  /**< Set the MAC for this device        */
#define ETH_CTRL_GET_MAC     3  /**< Get the MAC for this device        */
#define ETH_CTRL_SET_LOOPBK  4  /**< Set Loopback Mode                  */
#define ETH_CTRL_RESET       5  /**< Reset the Ethernet device          */
#define ETH_CTRL_DISABLE     6  /**< Disable the Ethernet device        */

/**
 * Ethernet packet buffer
 */
struct ethPktBuffer
{
    uchar *buf;                 /**< Pointer to buffer space            */
    uchar *data;                /**< Start of data within buffer        */
    int length;                 /**< Length of packet data              */
};

/* Ethernet control block */
#define ETH_INVALID  (-1)       /**< Invalid data (virtual devices)     */

/**
 * Ethernet control block
 */
struct ether
{
    uchar state;                /**< ETH_STATE_*above                   */
    device *phy;                /**< physical eth device for Tx DMA     */

    /* Pointers to associated structures */
    device *dev;                /**< eth device structure               */
    void *csr;                  /**< control and status registers       */

    ulong interruptMask;        /**< interrupt mask                     */
    ulong interruptStatus;      /**< interrupt status                   */

    struct dmaDescriptor *rxRing; /**< array of receiving ring descs.   */
    struct ethPktBuffer **rxBufs; /**< Rx ring array                    */
    ulong rxHead;               /**< Rx ring head index                 */
    ulong rxTail;               /**< Rx ring tail index                 */
    ulong rxRingSize;           /**< Number of Rx ring descriptors      */
    ulong rxirq;                /**< Count of Rx interrupt requests     */
    ulong rxOffset;             /**< Size in bytes of rxHeader          */
    ulong rxErrors;             /**< Count of Rx errors.                */

    struct dmaDescriptor *txRing; /**< array of transmit ring descs.    */
    struct ethPktBuffer **txBufs; /**< Tx ring array                    */
    ulong txHead;               /**< Tx ring head index                 */
    ulong txTail;               /**< Tx ring tail index                 */
    ulong txRingSize;           /**< Number of Tx ring descriptors      */
    ulong txirq;                /**< Count of Tx interrupt requests     */

    uchar devAddress[ETH_ADDR_LEN];

    uchar addressLength;        /**< Hardware address length            */
    ushort mtu;                 /**< Maximum transmission units         */

    ulong errors;               /**< Number of Ethernet errors          */
    ushort ovrrun;              /**< Buffer overruns                    */
    semaphore isema;            /**< I/0 sem for eth input              */
    ushort istart;              /**< Index of first byte                */
    ushort icount;              /**< Packets in buffer                  */

    struct ethPktBuffer *in[ETH_IBLEN]; /**< Input buffer               */

    int inPool;                 /**< buffer pool id for input           */
    int outPool;                /**< buffer pool id for output          */
};

extern struct ether ethertab[];

/* Driver functions */
devcall etherInit(device *);
devcall etherOpen(device *);
devcall etherClose(device *);
devcall etherRead(device *, void *, uint);
devcall etherWrite(device *, void *, uint);
devcall etherControl(device *, int, long, long);
interrupt etherInterrupt(void);

void etherStat(ushort);
void etherThroughput(ushort);

int colon2mac(char *, uchar *);
int allocRxBuffer(struct ether *, int);
int waitOnBit(volatile uint *, uint, const int, int);

#endif                          /* _ETHER_H_ */
