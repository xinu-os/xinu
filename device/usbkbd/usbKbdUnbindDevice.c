/**
 * @file usbUnbindDevice.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <usb_core_driver.h>
#include <usbkbd.h>

/**
 * Function called by the USB subsystem when a keyboard bound with
 * usbKbdBindDevice() has been detached.
 */
void usbKbdUnbindDevice(struct usb_device *dev)
{
    struct usbkbd *kbd;

    USBKBD_TRACE("USB keyboard disconnected (%s %s: address %u)",
                 dev->manufacturer, dev->product, dev->address);
    kbd = dev->driver_private;
    kbd->attached = FALSE;
}
