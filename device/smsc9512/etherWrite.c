/**
 * @file etherWrite.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include "smsc9512.h"
#include <bufpool.h>
#include <ether.h>
#include <interrupt.h>
#include <string.h>
#include <usb_core_driver.h>

/* Implementation of etherWrite() for the SMSC LAN9512; see the documentation
 * for this function in ether.h.  */
devcall etherWrite(device *devptr, const void *buf, uint len)
{
    struct ether *ethptr;
    struct usb_xfer_request *req;
    uint8_t *sendbuf;
    uint32_t tx_cmd_a, tx_cmd_b;

    ethptr = &ethertab[devptr->minor];
    if (ethptr->state != ETH_STATE_UP ||
        len < ETH_HEADER_LEN || len > ETH_HDR_LEN + ETH_MTU)
    {
        return SYSERR;
    }

    /* Get a buffer for the packet.  (This may block.)  */
    req = bufget(ethptr->outPool);

    /* Copy the packet's data into the buffer, but also include two words at the
     * beginning that contain device-specific flags.  These two fields are
     * required, although we essentially just use them to tell the hardware we
     * are transmitting one (1) packet with no extra bells and whistles.  */
    sendbuf = req->sendbuf;
    tx_cmd_a = len | TX_CMD_A_FIRST_SEG | TX_CMD_A_LAST_SEG;
    sendbuf[0] = (tx_cmd_a >> 0)  & 0xff;
    sendbuf[1] = (tx_cmd_a >> 8)  & 0xff;
    sendbuf[2] = (tx_cmd_a >> 16) & 0xff;
    sendbuf[3] = (tx_cmd_a >> 24) & 0xff;
    tx_cmd_b = len;
    sendbuf[4] = (tx_cmd_b >> 0)  & 0xff;
    sendbuf[5] = (tx_cmd_b >> 8)  & 0xff;
    sendbuf[6] = (tx_cmd_b >> 16) & 0xff;
    sendbuf[7] = (tx_cmd_b >> 24) & 0xff;
    STATIC_ASSERT(SMSC9512_TX_OVERHEAD == 8);
    memcpy(sendbuf + SMSC9512_TX_OVERHEAD, buf, len);

    /* Set total size of the data to send over the USB.  */
    req->size = len + SMSC9512_TX_OVERHEAD;

    /* Submit the data as an asynchronous bulk USB transfer.  In other words,
     * this tells the USB subsystem to send begin sending the data over the USB
     * to the SMSC LAN9512 USB Ethernet Adapter.  At some later time when all
     * the data has been transferred over the USB, smsc9512_tx_complete() will
     * be called by the USB subsystem.  */
    usb_submit_xfer_request(req);

    /* Return the length of the packet written (not including the
     * device-specific fields that were added). */
    return len;
}
