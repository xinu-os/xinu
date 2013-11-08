/**
 * @file usbBindDevice.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <usb_core_driver.h>
#include <usbkbd.h>

/* Constants defined by USB HID v1.11 specification  */
#define HID_SUBCLASS_BOOT           1     /* Section 4.2   */
#define HID_BOOT_PROTOCOL_KEYBOARD  1     /* Section 4.3   */
#define HID_REQUEST_SET_PROTOCOL    0x0B  /* Section 7.2   */
#define HID_BOOT_PROTOCOL           0     /* Section 7.2.5 */

/**
 * Function called by the USB subsystem when a USB device that may or may not be
 * a supported keyboard has been attached.
 */
usb_status_t
usbKbdBindDevice(struct usb_device *dev)
{
    uint i, j;
    const struct usb_interface_descriptor *kbd_interface;
    const struct usb_endpoint_descriptor *in_interrupt_endpoint;
    struct usbkbd *kbd;
    usb_status_t status;

    USBKBD_TRACE("Attempting to bind USB device (%s %s: address %u)",
                 dev->manufacturer, dev->product, dev->address);

    /* HID class is per-interface and cannot be in the device descriptor.  */
    if (USB_CLASS_CODE_INTERFACE_SPECIFIC != dev->descriptor.bDeviceClass)
    {
        USBKBD_TRACE("Device does not have interface specific class: "
                     "can't have any HID interfaces!");
        return USB_STATUS_DEVICE_UNSUPPORTED;
    }

    USBKBD_TRACE("Looking for HID interface supporting keyboard boot protocol");
    kbd_interface = NULL;
    for (i = 0; i < dev->config_descriptor->bNumInterfaces; i++)
    {
        struct usb_interface_descriptor *interface = dev->interfaces[i];

        USBKBD_TRACE("Examining interface (index=%u, bInterfaceNumber=%u)",
                     i, interface->bInterfaceNumber);
        if (USB_CLASS_CODE_HID != interface->bInterfaceClass)
        {
            USBKBD_TRACE("Not a HID interface");
            continue;
        }

        /* Check if the keyboard boot protocol is supported  */
        if (HID_SUBCLASS_BOOT != interface->bInterfaceSubClass ||
            HID_BOOT_PROTOCOL_KEYBOARD != interface->bInterfaceProtocol)
        {
            USBKBD_TRACE("Interface does not support keyboard boot protocol");
            continue;
        }

        /* Find the IN interrupt endpoint.  */
        in_interrupt_endpoint = NULL;
        for (j = 0; j < interface->bNumEndpoints; j++)
        {
            if ((dev->endpoints[i][j]->bmAttributes & 0x3) ==
                USB_TRANSFER_TYPE_INTERRUPT
                && (dev->endpoints[i][j]->bEndpointAddress >> 7) ==
                USB_DIRECTION_IN)
            {
                in_interrupt_endpoint = dev->endpoints[i][j];
                break;
            }
        }
        if (NULL == in_interrupt_endpoint)
        {
            USBKBD_TRACE("Interface has no IN interrupt endpoint");
            continue;
        }

        USBKBD_TRACE("Interface is supported");
        kbd_interface = interface;
        break;
    }
    if (NULL == kbd_interface)
    {
        USBKBD_TRACE("No HID interface with keyboard boot protocol found");
        return USB_STATUS_DEVICE_UNSUPPORTED;
    }

    /* Map this newly attached USB keyboard to a keyboard structure.  */
    kbd = NULL;
    for (i = 0; i < NUSBKBD; i++)
    {
        if (usbkbds[i].initialized && !usbkbds[i].attached)
        {
            USBKBD_TRACE("Using USB keyboard control block %u", i);
            kbd = &usbkbds[i];
            break;
        }
    }
    if (NULL == kbd)
    {
        USBKBD_TRACE("All initialized keyboard devices already in use");
        return USB_STATUS_DEVICE_UNSUPPORTED;
    }

    /* Put keyboard in boot protocol mode, not report mode.  */
    {
        usb_status_t status;

        USBKBD_TRACE("Placing keyboard in boot protocol mode");
        status = usb_control_msg(dev, NULL, HID_REQUEST_SET_PROTOCOL,
                                 USB_BMREQUESTTYPE_TYPE_CLASS |
                                     USB_BMREQUESTTYPE_DIR_OUT |
                                     USB_BMREQUESTTYPE_RECIPIENT_INTERFACE,
                                 HID_BOOT_PROTOCOL,
                                 kbd_interface->bInterfaceNumber, NULL, 0);
        if (USB_STATUS_SUCCESS != status)
        {
            USBKBD_TRACE("Failed to place keyboard in boot protocol mode: %s",
                         usb_status_string(status));
            return USB_STATUS_DEVICE_UNSUPPORTED;
        }
    }

    dev->driver_private = kbd;

    /* Initialize and submit the transfer request to the keyboard's IN interrupt
     * endpoint.  When this request completes, usbKbdInterrupt() will be called;
     * ordinarily this happens when a key is pressed or released, thereby
     * causing new data to be sent.  */
    kbd->intr->dev = dev;
    kbd->intr->endpoint_desc = in_interrupt_endpoint;
    kbd->intr->completion_cb_func = usbKbdInterrupt;
    kbd->intr->private = kbd;

    USBKBD_TRACE("Asking keyboard for data");

    status = usb_submit_xfer_request(kbd->intr);
    if (USB_STATUS_SUCCESS != status)
    {
        return status;
    }
    kbd->attached = TRUE;

    return USB_STATUS_SUCCESS;
}
