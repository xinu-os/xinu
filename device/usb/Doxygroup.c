/**
 * @defgroup usb USB
 * @ingroup devices
 * @brief Universal Serial Bus 2.0 subsystem
 *
 * @section usb_support USB support
 * 
 * This documentation describes Xinu's platform-independent USB (Universal
 * Serial Bus) code.  USB is a fully dynamic bus for connecting essentially any
 * arbitrary device to a computer.
 * 
 * This code, while significantly simpler than the equivalent code in other
 * operating systems such as Linux, is still quite complicated.  This is
 * unavoidable and is a result of the complicated design of USB itself.  Indeed,
 * the USB 2.0 specification is 650 pages, yet only includes a fraction of the
 * information that's needed to actually implement full USB support from scratch.
 * 
 * To include support for USB in Xinu, define @c WITH_USB in @c xinu.conf and
 * add @c usb to the @c DEVICES variable in @c platformVars.  Note that the \ref
 * usbhub "USB hub driver" will be included automatically as it is required for
 * USB to support any devices whatsoever.
 * 
 * In addition, you need to ensure that an appropriate Host Controller Driver,
 * which is platform-dependent code and is not located in this directory, has
 * been written and is compiled into the kernel.  For example,
 * system/platforms/arm-rpi/usb_dwc_hcd.c is the Host Controller Driver that is
 * used on the Raspberry Pi hardware.
 * 
 * Finally, you need to enable any actual USB devices you want to support by
 * adding the corresponding device directories to the @c DEVICES variable in @c
 * platformVars, then defining the appropriate static devices in @c xinu.conf.
 * For example, on the Raspberry Pi, we enable the driver for the SMSC LAN9512
 * USB Ethernet Adapter, which is located in @c device/smsc9512, by adding @c
 * smsc9512 to @c DEVICES and defining the @c ETH0 device in @c xinu.conf.
 * 
 * @section usb_writing_dd How to write a USB device driver
 * 
 * USB device drivers are implemented using the API provided by the \ref usbcore
 * "USB Core Driver".  This API is declared in include/usb_core_driver.h and
 * allows drivers to register themselves, bind themselves to USB devices that
 * are detected by the core, and communicate with USB devices.
 * 
 * See @c device/smsc9512 for an example of a USB device driver.
 * 
 * Note that Xinu's static device model is incompatible with USB's dynamic
 * device model, which is something that needs to be worked around by the USB
 * device drivers.  For example, the driver might refuse to bind itself to more
 * than a fixed number of USB devices, and it might block or return failure if
 * code tries to open the static device before it has actually been bound to an
 * actual USB device.
 * 
 * @section usb_writing_hcd How to write a USB Host Controller Driver
 * 
 * The USB Host Controller Driver is responsible for actually interacting with
 * the hardware (the USB Host Controller) to send and receive data over the USB.
 * Unfortunately, USB Host Controllers are not standardized by the USB
 * specification itself, which is why this abstraction layer is needed.  USB
 * Host Controllers include those compliant with the UHCI, OHCI, or EHCI
 * specifications, as well as nonstandard ones such as the Synopsys DesignWare
 * High-Speed USB 2.0 OTG Controller used on the Raspberry Pi.
 * 
 * The very first step is to determine whether Xinu already supports the USB Host
 * Controller for the hardware under consideration.  If so, you can use that code,
 * but some changes may be needed (e.g. the location of memory-mapped registers).
 * Otherwise, read on....
 * 
 * The USB Host Controller Driver must implement the interface declared in
 * include/usb_hcdi.h.
 * 
 * You first must acquire any documentation (if it exists) for the Host
 * Controller.  You also need to read relevant parts of the USB 2.0
 * Specification, mainly those that describe control, interrupt, and bulk
 * transfers.  Most of the 650 pages you do _not_ need to read.
 * 
 * Next, in hcd_start(), you must write any code that is needed to prepare the
 * Host Controller to be ready to use.
 * 
 * The next and essentially final step is to implement
 * hcd_submit_xfer_request(), which is very difficult.  You should initially
 * focus on faking requests that are sent to the root hub.  These will include
 * various control transfers to and from the root hub's default endpoint as well
 * as an interrupt transfer from the root hub's status change endpoint.  Some
 * root hub requests can be handled entirely in software; others will need to
 * communicate with the Host Controller.  Next, you must support control
 * transfers to and from actual USB devices on the bus.  Finally, you must
 * support interrupt and bulk transfers.  These must be asynchronous and
 * interrupt-driven.  Note that the hub driver uses interrupt transfers in order
 * to detect port status changes; thus, it will be impossible to enumerate the
 * entire USB until interrupt transfers have been implemented.
 * 
 * You can use the usb_debug() and usb_dev_debug() macros to print debugging
 * messages.  Enable them by changing the logging priorities in usb_util.h.
 * 
 * The @ref xsh_usbinfo() "usbinfo" shell command prints out information about
 * devices attached to the USB.
 *
 * @section usb_embedded USB for Embedded Systems
 *
 * For fully embedded systems where debugging facilities are not critical,
 * unnecessary human-friendly functionality can be omitted from the USB core.
 * See usbdebug.c for more details.
 * 
 * @defgroup usbcore USB Core Driver
 * @brief USB device model and API for USB device drivers
 * @ingroup usb
 *
 * @defgroup usbhub USB Hub Driver
 * @brief Driver for USB hub devices
 * @ingroup usb
 *
 * @defgroup usbhcd USB Host Controller Driver
 * @brief Platform-dependent code to actually send and receive data over the USB
 * @ingroup usb
 */
