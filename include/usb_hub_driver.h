/**
 * @file usb_hub_driver.h
 * @ingroup usbhub
 *
 * Interface to the USB hub driver.  This is intended to be used only by the USB
 * core driver.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#ifndef _USB_HUB_DRIVER_H_
#define _USB_HUB_DRIVER_H_

#include <usb_util.h>
#include <usb_core_driver.h>

void usb_hub_for_device_in_tree(struct usb_device *dev,
                                usb_status_t (*callback)(struct usb_device *));

extern const struct usb_device_driver usb_hub_driver;

#endif /* _USB_HUB_DRIVER_H_ */
