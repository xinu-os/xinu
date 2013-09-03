/**
 * @file usb_subsystem.h
 * @ingroup usb
 *
 * Interface to the USB (Universal Serial Bus) subsystem from other operating
 * system code.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#ifndef _USB_SUBSSYSTEM_H_
#define _USB_SUBSSYSTEM_H_

syscall usbinit(void);
syscall usbinfo(void);

#endif /* _USB_SUBSSYSTEM_H_ */
