/**
 * @file usbKbdInit.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <usb_core_driver.h>
#include <usbkbd.h>

/** Table of USB keyboard control blocks  */
struct usbkbd usbkbds[NUSBKBD];

/** USB device driver structure for the usbkbd driver  */
static struct usb_device_driver usbkbd_driver = {
    .name          = "USB keyboard driver (HID boot protocol)",
    .bind_device   = usbKbdBindDevice,
    .unbind_device = usbKbdUnbindDevice,
};

/**
 * Initializes the specified USB keyboard.
 *
 * This actually only prepares the corresponding keyboard structure for use and
 * does not depend on a physical keyboard being attached.  The physical keyboard
 * is recognized only when it is attached, and any read requests to the device
 * will block until that point.
 */
devcall usbKbdInit(device *devptr)
{
    usb_status_t status;
    struct usbkbd *kbd;

    kbd = &usbkbds[devptr->minor];

    /* Already initialized?  */
    if (kbd->initialized)
    {
        goto err;
    }

    /* Initialize input queue.  */
    kbd->isema = semcreate(0);
    if (SYSERR == kbd->isema)
    {
        goto err;
    }

    /* Allocate USB transfer request for keyboard data.  */
    kbd->intr = usb_alloc_xfer_request(8);
    if (NULL == kbd->intr)
    {
        goto err_semfree;
    }

    kbd->initialized = TRUE;

    /* Register the keyboard USB device driver with the USB subsystem.
     * (no-op if already registered).  */
    status = usb_register_device_driver(&usbkbd_driver);
    if (status != USB_STATUS_SUCCESS)
    {
        goto err_free_req;
    }

    return OK;

err_free_req:
    kbd->initialized = FALSE;
    usb_free_xfer_request(kbd->intr);
err_semfree:
    semfree(kbd->isema);
err:
    return SYSERR;
}
