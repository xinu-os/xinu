/**
 * @file etherInit.c
 *
 * Initialization for the SMSC LAN9512 USB Ethernet Adapter.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include "smsc9512.h"
#include <clock.h>
#include <ether.h>
#include <memory.h>
#include <platform.h>
#include <semaphore.h>
#include <stdlib.h>
#include <usb_core_driver.h>

/* Global table of Ethernet devices.  */
struct ether ethertab[NETHER];

/**
 * Semaphores that indicate whether each given Ethernet device has actually been
 * connected to the system yet (here, attached to the USB).  Could be moved into
 * <code>struct ::ether</code> if other drivers needed it...
 */
static semaphore smsc9512_attached[NETHER];

/**
 * Try to bind the SMSC LAN9512 driver to a specific USB device.  This the @ref
 * usb_device_driver::bind_device "bind_device" implementation for the SMSC
 * LAN9512 driver and therefore complies with its documented behavior.
 */
static usb_status_t
smsc9512_bind_device(struct usb_device *udev)
{
    struct ether *ethptr;

    /* Check if this is actually a SMSC LAN9512 by checking the USB device's
     * standard device descriptor, which the USB core already read into memory.
     * Also check to make sure the expected endpoints for sending/receiving
     * packets are present and that the device is operating at high speed.  */
    if (udev->descriptor.idVendor != SMSC9512_VENDOR_ID ||
        udev->descriptor.idProduct != SMSC9512_PRODUCT_ID ||
        udev->interfaces[0]->bNumEndpoints < 2 ||
        (udev->endpoints[0][0]->bmAttributes & 0x3) != USB_TRANSFER_TYPE_BULK ||
        (udev->endpoints[0][1]->bmAttributes & 0x3) != USB_TRANSFER_TYPE_BULK ||
        (udev->endpoints[0][0]->bEndpointAddress >> 7) != USB_DIRECTION_IN ||
        (udev->endpoints[0][1]->bEndpointAddress >> 7) != USB_DIRECTION_OUT ||
        udev->speed != USB_SPEED_HIGH)
    {
        return USB_STATUS_DEVICE_UNSUPPORTED;
    }

    /* Make sure this driver isn't already bound to a SMSC LAN9512.
     * TODO: Support multiple devices of this type concurrently.  */
    ethptr = &ethertab[0];
    STATIC_ASSERT(NETHER == 1);
    if (ethptr->csr != NULL)
    {
        return USB_STATUS_DEVICE_UNSUPPORTED;
    }

    /* The rest of this function is responsible for making the SMSC LAN9512
     * ready to use, but not actually enabling Rx and Tx (which is done in
     * etherOpen()).  This primarily involves writing to the registers on the
     * SMSC LAN9512.  But these are not memory mapped registers, as this is a
     * USB Ethernet Adapter that is attached on the USB!  Instead, registers are
     * read and written using USB control transfers.  It's somewhat of a pain,
     * and also unlike memory accesses it is possible for USB control transfers
     * to fail.  However, here we perform lazy error checking where we just do
     * all the needed reads and writes, then check at the end if an error
     * occurred.  */

    udev->last_error = USB_STATUS_SUCCESS;

    /* Resetting the SMSC LAN9512 via its registers should not be necessary
     * because the USB code already performed a reset on the USB port it's
     * attached to.  */

    /* Set MAC address.  */
    smsc9512_set_mac_address(udev, ethptr->devAddress);

    /* Allow multiple Ethernet frames to be received in a single USB transfer.
     * Also set a couple flags of unknown function.  */
    smsc9512_set_reg_bits(udev, HW_CFG, HW_CFG_MEF | HW_CFG_BIR | HW_CFG_BCE);

    /* Set the maximum USB (not networking!) packets per USB Rx transfer.
     * Required when HW_CFG_MEF was set.  */
    smsc9512_write_reg(udev, BURST_CAP,
                       SMSC9512_DEFAULT_HS_BURST_CAP_SIZE / SMSC9512_HS_USB_PKT_SIZE);

    /* Check for error and return.  */
    if (udev->last_error != USB_STATUS_SUCCESS)
    {
        return udev->last_error;
    }
    ethptr->csr = udev;
    udev->driver_private = ethptr;
    signal(smsc9512_attached[ethptr - ethertab]);
    return USB_STATUS_SUCCESS;
}

/**
 * Unbinds the SMSC LAN9512 driver from a SMSC LAN9512 that has been detached.
 * This the @ref usb_device_driver::unbind_device "unbind_device" implementation
 * for the SMSC LAN9512 driver and therefore complies with its documented
 * behavior.
 */
static void
smsc9512_unbind_device(struct usb_device *udev)
{
    struct ether *ethptr = udev->driver_private;

    /* Reset attached semaphore to 0.  */
    wait(smsc9512_attached[ethptr - ethertab]);

    /* Close the device.  */
    etherClose(ethptr->dev);
}

/**
 * Specification of a USB device driver for the SMSC LAN9512.  This is
 * specifically for the USB core and is not related to Xinu's primary device and
 * driver model, which is static.
 */
static const struct usb_device_driver smsc9512_driver = {
    .name          = "SMSC LAN9512 USB Ethernet Adapter Driver",
    .bind_device   = smsc9512_bind_device,
    .unbind_device = smsc9512_unbind_device,
};

static void
randomEthAddr(uchar addr[ETH_ADDR_LEN])
{
    uint i;
    if (platform.serial_low != 0 && platform.serial_high != 0)
    {
        /* Use value generated from platform's serial number.  The problem here
         * is that we must generate a 48-bit MAC address from a 64-bit serial
         * number but avoid mapping multiple serial numbers to the same MAC
         * address.  This is impossible, so we perform an approximation where we
         * hash the serial number to remove any obvious non-randomness in the
         * way serial numbers are assigned, then extract the low 48 bits.  */
        unsigned long long serial_nr, hashval;
        
        serial_nr = (unsigned long long)platform.serial_low |
                    ((unsigned long long)platform.serial_high << 32);
        hashval = serial_nr * 0x9e37fffffffc0001ULL;
        for (i = 0; i < ETH_ADDR_LEN; i++)
        {
            addr[i] = hashval & 0xff;
            hashval >>= 8;
        }
    }
    else
    {
        /* Generate value using the C library's random number generator, seeded
         * on the current system timer tick count.  */
        srand(clkcount());
        for (i = 0; i < ETH_ADDR_LEN; i++)
        {
            addr[i] = rand();
        }
    }
    /* Clear multicast bit and set locally assigned bit */
    addr[0] &= 0xfe;
    addr[0] |= 0x02;
}

/**
 * @ingroup etherspecific
 *
 * Wait until the specified Ethernet device has actually been attached.
 *
 * This is necessary because USB is a dynamic bus, but Xinu expects static
 * devices.  Therefore, code opening a static ethernet device must wait until
 * the corresponding USB device has actually been detected and attached.  Fun
 * fact: the USB standard places no constraints on how long this will actually
 * take, even if the device is physically soldered to the board.
 *
 * TODO: Wait for at most a certain amount of time before returning failure.
 *
 * @param minor
 *     Minor number of the Ethernet device to wait for.
 *
 * @return
 *      Currently ::USB_STATUS_SUCCESS.  TODO: implement timeouts and return
 *      ::USB_STATUS_TIMEOUT if timed out.
 */
usb_status_t
smsc9512_wait_device_attached(ushort minor)
{
    wait(smsc9512_attached[minor]);
    signal(smsc9512_attached[minor]);
    return USB_STATUS_SUCCESS;
}

/* Implementation of etherInit() for the smsc9512; see the documentation for
 * this function in ether.h.  */
/**
 * @details
 * 
 * SMSC LAN9512-specific notes:  This function returns ::OK if the Ethernet
 * driver was successfully registered with the USB core, otherwise ::SYSERR.
 * This is a work-around to use USB's dynamic device model at the same time as
 * Xinu's static device model, and there is no guarantee that the device
 * actually exists when this function returns.  (If it doesn't, the problem is
 * delayed until the device is actually opened with etherOpen()).
 */
devcall etherInit(device *devptr)
{
    struct ether *ethptr;
    usb_status_t status;

    /* Initialize the static `struct ether' for this device.  */
    ethptr = &ethertab[devptr->minor];
    bzero(ethptr, sizeof(struct ether));
    ethptr->dev = devptr;
    ethptr->state = ETH_STATE_DOWN;
    ethptr->mtu = ETH_MTU;
    ethptr->addressLength = ETH_ADDR_LEN;
    ethptr->isema = semcreate(0);
    if (isbadsem(ethptr->isema))
    {
        goto err;
    }

    smsc9512_attached[devptr->minor] = semcreate(0);

    if (isbadsem(smsc9512_attached[devptr->minor]))
    {
        goto err_free_isema;
    }

    /* The SMSC LAN9512 on the Raspberry Pi does not have an EEPROM attached.
     * The EEPROM is normally used to store the MAC address of the adapter,
     * along with some other information.  As a result, software needs to set
     * the MAC address to a value of its choosing (such as a random number).  */
    randomEthAddr(ethptr->devAddress);

    /* Register this device driver with the USB core and return.  */
    status = usb_register_device_driver(&smsc9512_driver);
    if (status != USB_STATUS_SUCCESS)
    {
        goto err_free_attached_sema;
    }
    return OK;

err_free_attached_sema:
    semfree(smsc9512_attached[devptr->minor]);
err_free_isema:
    semfree(ethptr->isema);
err:
    return SYSERR;
}
