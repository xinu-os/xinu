/**
 * @file ether.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

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

/**
 * \ingroup ether
 *
 * Global table of ethernet devices.  Note that Xinu does not support multiple
 * types of Ethernet devices concurrently, only all the same kind.
 */
extern struct ether ethertab[];

/* Ethernet driver functions */

/**
 * \ingroup ether
 *
 * Do one-time initialization of an Ethernet device.  This performs
 * initialization of the Ethernet device that perists through calls to
 * etherOpen() and etherClose(), such as initializing the Ethernet control block
 * and setting hardware parameters.
 *
 * @param devptr
 *      Pointer to the entry in Xinu's device table for the Ethernet device.
 * @return
 *      ::OK if the device was initialized successfully; otherwise ::SYSERR.
 */
devcall etherInit(device *devptr);

/**
 * \ingroup ether
 *
 * Open an Ethernet device.  This enables Tx and Rx functionality and sets the
 * device state to ::ETH_STATE_UP.  This should be called through open().
 *
 * @param devptr
 *      Pointer to the entry in Xinu's device table for the Ethernet device.
 *
 * @return
 *      ::OK if the device was opened successfully; otherwise ::SYSERR.
 */
devcall etherOpen(device *devptr);

/**
 * \ingroup ether
 *
 * Close an Ethernet device.  This disables Tx and Rx functionality and sets the
 * device state to ::ETH_STATE_DOWN.  This should be called through close().
 *
 * @param devptr
 *      Pointer to the entry in Xinu's device table for the Ethernet device.
 *
 * @return ::OK if the device was closed successfully; otherwise ::SYSERR.
 */
devcall etherClose(device *devptr);

/**
 * \ingroup ether
 *
 * Read an Ethernet frame from an Ethernet device.  This should be called
 * through read().
 *
 * This function blocks until a frame has actually been received.  There is no
 * timeout.
 *
 * @param devptr
 *      Pointer to the entry in Xinu's device table for the Ethernet device.
 * @param buf
 *      Buffer in which to receive the Ethernet frame.  The received frame will
 *      start with the MAC destination address and end with the payload.
 * @param len
 *      Maximum length, in bytes, of the Ethernet frame to receive (size of @p
 *      buf).
 *
 * @return
 *      ::SYSERR if the Ethernet device is not currently up; otherwise the
 *      actual length of the Ethernet frame received and written to @p buf.
 */
devcall etherRead(device *devptr, void *buf, uint len);

/**
 * \ingroup ether
 *
 * Write an Ethernet frame (excluding certain fields) to an Ethernet device.
 * This should be called through write().
 *
 * This function actually only buffers the frame to be sent at some later time.
 * Therefore, there is no guarantee the frame has actually been transmitted on
 * the wire when this function returns.
 *
 * @param devptr
 *      Pointer to the entry in Xinu's device table for the Ethernet device.
 * @param buf
 *      Buffer that contains the Ethernet frame to send.  It must start with the
 *      MAC destination address and end with the payload.
 * @param len
 *      Length, in bytes, of the Ethernet frame to send.
 *
 * @return
 *      ::SYSERR if packet is too small, too large, or the Ethernet device is
 *      not currently up; otherwise @p len, the number of bytes submitted to be
 *      written at some later time.
 */
devcall etherWrite(device *devptr, const void *buf, uint len);

/**
 * \ingroup ether
 *
 * Execute a control function on an Ethernet device.  This should be called
 * through control().
 *
 * @param devptr
 *      Pointer to the entry in Xinu's device table for the Ethernet device.
 * @param req
 *      Control request to execute.
 * @param arg1
 *      First argument (if any) for the control request.
 * @param arg2
 *      Second argument (if any) for the control request.
 *
 * @return
 *      The result of the control request, or ::SYSERR if the control request
 *      @p req was not recognized.
 */
devcall etherControl(device *devptr, int req, long arg1, long arg2);

/**
 * \ingroup ether
 *
 * Print information about an Ethernet device.
 *
 * @param minor
 *      Minor number of the Ethernet device to print information about.
 */
void etherStat(ushort minor);

/**
 * \ingroup ether
 *
 * Print throughput data about an Ethernet device.
 *
 * @param minor
 *      Minor number of the Ethernet device to print throughput data about.
 */
void etherThroughput(ushort minor);

interrupt etherInterrupt(void);

/**
 * \ingroup ether
 */
int colon2mac(char *, uchar *);
int allocRxBuffer(struct ether *, int);
int waitOnBit(volatile uint *, uint, const int, int);

#endif                          /* _ETHER_H_ */
