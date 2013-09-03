/**
 * @file etherOpen.c
 *
 * Code for opening a SMSC LAN9512 USB Ethernet Adapter device.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include "smsc9512.h"
#include <bufpool.h>
#include <ether.h>
#include <stdlib.h>
#include <string.h>
#include <usb_core_driver.h>

/* Implementation of etherOpen() for the smsc9512; see the documentation for
 * this function in ether.h.  */
/**
 * @details
 *
 * SMSC LAN9512-specific notes:  as a work-around to use USB's dynamic device
 * model at the same time as Xinu's static device model, this function will
 * block until the corresponding USB device has actually been attached to the
 * USB.  Strictly speaking, there is no guarantee as to when this will actually
 * occur, even if the device is non-removable.
 */
devcall etherOpen(device *devptr)
{
    struct ether *ethptr;
    struct usb_device *udev;
    irqmask im;
    uint i;
    int retval = SYSERR;

    im = disable();

    /* Wait for USB device to actually be attached.  */
    if (smsc9512_wait_device_attached(devptr->minor) != USB_STATUS_SUCCESS)
    {
        goto out_restore;
    }

    /* Fail if device is not down.  */
    ethptr = &ethertab[devptr->minor];
    if (ethptr->state != ETH_STATE_DOWN)
    {
        goto out_restore;
    }

    /* Create buffer pool for Tx transfers.  */
    ethptr->outPool = bfpalloc(sizeof(struct usb_xfer_request) + ETH_MAX_PKT_LEN +
                                   SMSC9512_TX_OVERHEAD,
                               SMSC9512_MAX_TX_REQUESTS);
    if (ethptr->outPool == SYSERR)
    {
        goto out_restore;
    }

    /* Create buffer pool for Rx packets (not the actual USB transfers, which
     * are allocated separately).  */
    ethptr->inPool = bfpalloc(sizeof(struct ethPktBuffer) + ETH_MAX_PKT_LEN,
                              ETH_IBLEN);
    if (ethptr->inPool == SYSERR)
    {
        goto out_free_out_pool;
    }

    /* We're abusing the csr field to store a pointer to the USB device
     * structure.  At least it's somewhat equivalent, since it's what we need to
     * actually communicate with the device hardware.  */
    udev = ethptr->csr;

    /* Set MAC address */
    if (smsc9512_set_mac_address(udev, ethptr->devAddress) != USB_STATUS_SUCCESS)
    {
        goto out_free_in_pool;
    }

    /* Initialize the Tx requests.  */
    {
        struct usb_xfer_request *reqs[SMSC9512_MAX_TX_REQUESTS];
        for (i = 0; i < SMSC9512_MAX_TX_REQUESTS; i++)
        {
            struct usb_xfer_request *req;
            
            req = bufget(ethptr->outPool);
            usb_init_xfer_request(req);
            req->dev = udev;
            /* Assign Tx endpoint, checked in smsc9512_bind_device() */
            req->endpoint_desc = udev->endpoints[0][1];
            req->sendbuf = (u8*)req + sizeof(struct usb_xfer_request);
            req->completion_cb_func = smsc9512_tx_complete;
            req->private = ethptr;
            reqs[i] = req;
        }
        for (i = 0; i < SMSC9512_MAX_TX_REQUESTS; i++)
        {
            buffree(reqs[i]);
        }
    }

    /* Allocate and submit the Rx requests.  TODO: these aren't freed anywhere.
     * */
    for (i = 0; i < SMSC9512_MAX_RX_REQUESTS; i++)
    {
        struct usb_xfer_request *req;
        
        req = usb_alloc_xfer_request(SMSC9512_DEFAULT_HS_BURST_CAP_SIZE);
        if (req == NULL)
        {
            goto out_free_in_pool;
        }
        req->dev = udev;
        /* Assign Rx endpoint, checked in smsc9512_bind_device() */
        req->endpoint_desc = udev->endpoints[0][0];
        req->completion_cb_func = smsc9512_rx_complete;
        req->private = ethptr;
        usb_submit_xfer_request(req);
    }

    /* Enable transmit and receive on the actual hardware.  After doing this and
     * restoring interrupts, the Rx transfers can complete at any time due to
     * incoming packets.  */
    udev->last_error = USB_STATUS_SUCCESS;
    smsc9512_set_reg_bits(udev, MAC_CR, MAC_CR_TXEN | MAC_CR_RXEN);
    smsc9512_write_reg(udev, TX_CFG, TX_CFG_ON);
    if (udev->last_error != USB_STATUS_SUCCESS)
    {
        goto out_free_in_pool;
    }

    /* Success!  Set the device to ETH_STATE_UP. */
    ethptr->state = ETH_STATE_UP;
    retval = OK;
    goto out_restore;

out_free_in_pool:
    bfpfree(ethptr->inPool);
out_free_out_pool:
    bfpfree(ethptr->outPool);
out_restore:
    restore(im);
    return retval;
}
