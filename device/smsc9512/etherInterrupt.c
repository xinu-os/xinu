/**
 * @file     etherInterrupt.c
 *
 *
 * This file provides USB transfer completion callbacks for the SMSC LAN9512 USB
 * Ethernet Adapter.  These are roughly the equivalent of etherInterrupt() as
 * implemented in other Xinu Ethernet drivers, hence the filename, but there is
 * no actual etherInterrupt() function here because of how USB works.  The SMSC
 * LAN9512 cannot actually issue an interrupt by itself--- what actually happens
 * is that a USB transfer to or from it completes, thereby causing the USB Host
 * Controller to interrupt the CPU.  This interrupt is handled by the USB Host
 * Controller Driver, which will then call the callback function registered for
 * the USB transfer.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include "smsc9512.h"
#include <bufpool.h>
#include <ether.h>
#include <string.h>
#include <usb_core_driver.h>

/**
 * @ingroup etherspecific
 *
 * Callback function executed with interrupts disabled when an asynchronous USB
 * bulk transfer to the Bulk OUT endpoint of the SMSC LAN9512 USB Ethernet
 * Adapter for the purpose of sending an Ethernet packet has successfully
 * completed or has failed.
 *
 * Currently all this function has to do is return the buffer to its pool.  This
 * may wake up a thread in etherWrite() that is waiting for a free buffer.
 *
 * @param req
 *      USB bulk OUT transfer request that has completed.
 */
void smsc9512_tx_complete(struct usb_xfer_request *req)
{
    struct ether *ethptr = req->private;

    ethptr->txirq++;
    usb_dev_debug(req->dev, "SMSC9512: Tx complete\n");
    buffree(req);
}

/**
 * @ingroup etherspecific
 *
 * Callback function executed with interrupts disabled when an asynchronous USB
 * bulk transfer from the Bulk IN endpoint of the SMSC LAN9512 USB Ethernet
 * Adapter for the purpose of receiving one or more Ethernet packets has
 * successfully completed or has failed.
 *
 * This function is responsible for breaking up the raw USB transfer data into
 * the constituent Ethernet packet(s), then pushing them onto the incoming
 * packets queue (which may wake up threads in etherRead() that are waiting for
 * new packets).  It then must re-submit the USB bulk transfer request so that
 * packets can continue to be received.
 *
 * @param req
 *      USB bulk IN transfer request that has completed.
 */
void smsc9512_rx_complete(struct usb_xfer_request *req)
{
    struct ether *ethptr = req->private;

    ethptr->rxirq++;
    if (req->status == USB_STATUS_SUCCESS)
    {
        const u8 *data, *edata;
        u32 recv_status;
        u32 frame_length;

        /* For each Ethernet frame in the received USB data... */
        for (data = req->recvbuf, edata = req->recvbuf + req->actual_size;
             data + SMSC9512_RX_OVERHEAD + ETH_HDR_LEN + ETH_CRC_LEN <= edata;
             data += SMSC9512_RX_OVERHEAD + ((frame_length + 3) & ~3))
        {
            /* Get the Rx status word, which contains information about the next
             * Ethernet frame.  */
            recv_status = data[0] | data[1] << 8 | data[2] << 16 | data[3] << 24;

            /* Extract frame_length, which specifies the length of the next
             * Ethernet frame from the MAC destination address to end of the CRC
             * following the payload.  (This does not include the Rx status
             * word, which we instead account for in SMSC9512_RX_OVERHEAD.) */
            frame_length = (recv_status & RX_STS_FL) >> 16;

            if ((recv_status & RX_STS_ES) ||
                (frame_length + SMSC9512_RX_OVERHEAD > edata - data) ||
                (frame_length > ETH_MAX_PKT_LEN + ETH_CRC_LEN) ||
                (frame_length < ETH_HDR_LEN + ETH_CRC_LEN))
            {
                /* The Ethernet adapter set the error flag to indicate a problem
                 * or the Ethernet frame size it provided was invalid. */
                usb_dev_debug(req->dev, "SMSC9512: Tallying rx error "
                              "(recv_status=0x%08x, frame_length=%u)\n",
                              recv_status, frame_length);
                ethptr->errors++;
            }
            else if (ethptr->icount == ETH_IBLEN)
            {
                /* No space to buffer another received packet.  */
                usb_dev_debug(req->dev, "SMSC9512: Tallying overrun\n");
                ethptr->ovrrun++;
            }
            else
            {
                /* Buffer the received packet.  */

                struct ethPktBuffer *pkt;

                pkt = bufget(ethptr->inPool);
                pkt->buf = pkt->data = (u8*)(pkt + 1);
                pkt->length = frame_length - ETH_CRC_LEN;
                memcpy(pkt->buf, data + SMSC9512_RX_OVERHEAD, pkt->length);
                ethptr->in[(ethptr->istart + ethptr->icount) % ETH_IBLEN] = pkt;
                ethptr->icount++;

                usb_dev_debug(req->dev, "SMSC9512: Receiving "
                              "packet (length=%u, icount=%u)\n",
                              pkt->length, ethptr->icount);

                /* This may wake up a thread in etherRead().  */
                signal(ethptr->isema);
            }
        }
    }
    else
    {
        /* USB transfer failed for some reason.  */
        usb_dev_debug(req->dev, "SMSC9512: USB Rx transfer failed\n");
        ethptr->errors++;
    }
    usb_dev_debug(req->dev, "SMSC9512: Re-submitting USB Rx request\n");
    usb_submit_xfer_request(req);
}
