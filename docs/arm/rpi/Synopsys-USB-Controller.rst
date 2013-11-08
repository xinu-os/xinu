Synopsys DesignWare High Speed USB 2.0 On The Go Controller
===========================================================

The **Synopsys DesignWare High-Speed USB 2.0 On-The-Go Controller** is
the USB controller used on the :doc:`Raspberry-Pi`. This hardware is
notorious for having no official documentation available to end users
[#nodocs]_ and for having an extremely complicated, poorly written
Linux driver.  According to Greg Kroah-Hartman, the maintainer of
Linux's USB subsystem [#linux_maintainers]_:


    " ... it's just a really bad USB controller chip, combined with a
    sad way to hook it up to the processor, combined with with a truly
    horrible driver make for the fact that USB works at all on this
    board a total
    miracle." [#usb_sucks_quote]_

This page attempts to explain this hardware in the context of the USB
Host Controller Driver we wrote for it as part of the project to port
Embedded Xinu to the Raspberry Pi. Unfortunately, it is not intended to
*fully* document the hardware, since Embedded Xinu's driver is a
relatively simple, stripped-down driver that does not support many
features of the Linux driver.

Overview of Embedded Xinu's driver
----------------------------------

As mentioned, there is no documentation available for this USB Controller;
therefore, it obviously was difficult to implement a driver for it. Since there
was no other option, we had to gleam the relevant hardware details from other
drivers, mainly the Linux driver [#linux_driver]_, but also other drivers
written for the controller, such as the CSUD driver [#csud]_ and the Plan 9
driver [#plan9_driver]_. Our code, however, is a new implementation that is
intended to be simple and well-documented, and appropriate (to the extent
possible for USB and for this hardware) to include in a simple educational
operating system.

Embedded Xinu's driver supports control, interrupt, and bulk transfers.  As a
Host Controller Driver, it implements the interface declared in
:source:`include/usb_hcdi.h`.  For simplicity, Embedded Xinu's driver **does
not** support some features of the Linux driver, including but not limited to
the following:

-  Device mode. The "On-The-Go" portion of the hardware's name means it
   supports the `On-The-Go
   protocol <https://en.wikipedia.org/wiki/USB_On-The-Go|USB>`__, which
   is an extension to the main USB specification that allows the USB
   hardware to operate in either "host" or "device" mode. However, in
   our driver we are only concerned with host mode.
-  Isochronous transfers
-  Support for instantiations of the silicon other than the one used on
   the Raspberry Pi
-  Advanced transaction scheduling that takes into account special
   properties of periodic transfers
-  Various module parameters to configure the driver
-  Power management, including suspend and hibernation
-  Slave or Descriptor DMA modes

More details
------------

More details about the device and the registers may eventually be added here.
For now, see the source code (:source:`system/platforms/arm-rpi/usb_dwc_hcd.c`
and :source:`system/platforms/arm-rpi/usb_dwc_regs.h`), which is intended to be
easy to read and well documented. There is obviously a limit to how "easy to
read" it can be, though, since USB itself is very complicated.

Notes
-----

.. [#nodocs] http://www.raspberrypi.org/phpBB3/viewtopic.php?f=72&t=27695
.. [#linux_maintainers] http://lxr.linux.no/linux/MAINTAINERS
.. [#usb_sucks_quote] http://lists.infradead.org/pipermail/linux-rpi-kernel/2012-September/000214.html
.. [#linux_driver] https://github.com/raspberrypi/linux/tree/rpi-3.6.y/drivers/usb/host/dwc_otg
.. [#csud] https://github.com/Chadderz121/csud
.. [#plan9_driver] http://plan9.bell-labs.com/sources/plan9/sys/src/9/bcm/usbdwc.c
