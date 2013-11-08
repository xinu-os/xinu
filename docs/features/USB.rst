USB
===

**USB (Universal Serial Bus)** is a standard for connecting devices to a
computer system. It supports an immense range of devices, including (but
not limited to) keyboards, mice, flash drives, microphones, and network
adapters.

Although USB is ubiquitous in modern computer systems, even in some
"embedded" devices, it is very challenging to implement software support
for USB. This is primarily a result of the high complexity of USB, which
arises from several factors, including support for virtually any
arbitrary device, support for dynamic device attachment and detachment,
backwards compatibility with multiple versions of the USB specification,
and multiple supported speeds and transfer types. The USB 2.0
specification is 650 pages long, yet only covers a fraction of the
information needed to implement, from scratch, a USB software stack and
a driver controlling a specific USB device.

Due to the high complexity of USB, this article cannot fully explain
USB, nor can it even fully explain Embedded Xinu's implementation of
USB. Instead, it gives an overview of USB in the context of Embedded
Xinu's implementation. For full details about USB, the reader will
inevitably need to read the USB specification itself, as well as other
relevant specifications and webpages. For full details specifically
about Embedded Xinu's implementation, the reader will inevitably need to
read the source code.

.. contents::
   :local:

General USB Information
-----------------------

Bus Topology
~~~~~~~~~~~~

Fundamentally, USB is just a way to connect devices to a computer
system. A USB bus accomplishes this by arranging devices in a tree. Each
node of the tree is a **USB device**. There are two fundamental types of
USB devices: **hubs** and **functions**. USB hubs can have "child"
devices in the tree, while functions cannot. Hubs can be "children" of
other hubs, up to a depth of 7 levels.

The root node of the tree is the **root hub**, and every USB bus has one
(although it may be faked by the Host Controller Driver, described
later).

A USB hub provides a fixed number of attachment points for additional
devices called **ports**. A USB port may be either completely internal
or exposed to the "outside" as a place to plug in a USB cable. From the
user's point of view there is certainly a difference between these two
manifestations of a USB port, but from the software's point of view
there is no difference. On a similar note, it is also possible that a
single physical package, which a naive user might refer to as a "USB
device", actually contains an integrated USB hub onto which one or more
USB devices (as defined above) are attached. Such physical packages are
referred to as **compound devices**. An example of a compound device is
one of Apple's USB keyboards that provides a USB port to attach a mouse.

Since USB is a dynamic bus, USB devices can be attached or detached from
the USB at any arbitrary time. Detaching a hub implies detaching all
child devices.

For its part, Embedded Xinu's USB implementation fully supports the
dynamic tree topology of a USB bus.

Devices
~~~~~~~

Due to the generality of USB a USB device that is not a hub can be
virtually anything at all. This is made possible in part by a highly
nested design:

-  A USB device has one or more **configurations**.
-  A configuration has one or more **interfaces**.
-  An interface has one or more **alternate settings**.
-  An alternate setting has one or more **endpoints**.

Every device, configuration, interface, and endpoint has a corresponding
**descriptor** that can be read by the USB software to retrieve
information about the described entity in a standard format.

Although this format allows for highly complex devices, most devices are
relatively simple and have just one configuration. Furthermore, common
devices only have one interface. In fact, as of this writing, Embedded
Xinu's USB subsystem aims to support the common case only; it therefore
always sets the device to its first listed configuration, then attempts
to bind a device driver to the entire device rather than examining
individual interfaces to see if they need separate "interface drivers".

Host Controllers
~~~~~~~~~~~~~~~~

USB is a polled bus, so all transfers over the USB are initiated by the
**host**. The term "host" in this context means the USB software as well
as the USB Host Controller, which is the hardware responsible for
actually sending and receiving data over the USB and maintaining the
root hub. This is actually one of the trickier parts of USB. Since the
USB specification itself does not standardize the exact division of
tasks between software and hardware, it's often not clear who is
responsible when the specification says "host".

The essential thing to know is that the place where the USB software
directly meets the USB hardware is in the USB Host Controller Driver,
which operates the USB Host Controller. Some USB Host Controllers
present standard interfaces (such as UHCI, OHCI, or EHCI--- all
defined in specifications separate from the USB specification itself)
to software. Others do not present a standard interface, but instead
have vendor-provided documentation and/or a vendor-provided driver; an
example of this is the :doc:`/arm/rpi/Synopsys-USB-Controller` used on
the :doc:`/arm/rpi/Raspberry-Pi`.  Obviously, a standard interface is
highly preferred when independently implementing a Host Controller
Driver.

Transfers
~~~~~~~~~

To communicate with USB devices, the host sends and receives data over
the USB using USB transfers. A USB transfer occurs to or from a
particular endpoint on a particular device. Every endpoint is associated
with a specific type of USB transfer, which can be one of the following:

-  **Control** transfers. These are typically used for device
   configuration. There are two main unique features of these transfers.
   First, a special packet called SETUP is always sent over the USB
   before the actual data of the control transfer, and software needs to
   specify the contents of this packet. Second, every device has an
   endpoint over which control transfers in either direction can be
   made, and this endpoint is never explicitly listed in the endpoint
   descriptors.
-  **Interrupt** transfers. These are used for time-bounded transmission
   of small quantities of data (e.g. data from a keyboard or mouse).
-  **Bulk** transfers. These are used for reliable (with error
   detection) transmission of large quantities of data with no
   particular time guarantees (e.g. reading and writing data on mass
   storage devices).
-  **Isochronous** transfers. These are used for regular transmission of
   data with no error detecting (e.g. video capture).

Embedded Xinu currently supports control, interrupt, and bulk transfers.
Isochronous transfers have not yet been tested. Although currently
functional, interrupt transfers may require some more work to guarantee,
in all cases, the time-bounded transmission required by the USB
specification.

Speeds
~~~~~~

USB supports multiple transfer speeds:

-  1.5 Mbit/s (Low Speed) (USB 1+)
-  12 Mbit/s (Full Speed) (USB 1+)
-  480 Mbit/s (High Speed) (USB 2.0+)
-  5000 Mbit/s (Super Speed) (USB 3.0+)

Yes, Full Speed is in fact the second lowest speed. Well I think we all
know that 12 Mbit/s ought to be enough for anyone. But anyway, due to
the need to maintain backwards compatibility with legacy devices, the
USB software (mainly the host controller driver) unfortunately needs to
take into account transfer speeds. At minimum, it must be aware that
transfers to or from devices attached at Low Speed or Full Speed are
performed as a series of **split transactions**, which allow Low Speed
or Full Speed transfers to occur without significantly slowing down the
portion of the USB bus operating at a higher speed.

As of this writing, Embedded Xinu's USB subsystem supports USB 2.0, so
it supports devices operating at Low Speed, Full Speed, or High Speed.
USB 3.0 Super Speed is not supported.

.. _usb_subsystem:

Embedded Xinu's USB subsystem
-----------------------------

Now that some general information about USB has been presented, it
should be easier to understand the basic design of a USB software
stack.  The description that follows is certainly not the only way to
organize the code, but it is the way that is used in most operating
systems and makes the most sense based on how USB was designed. In
terms of Embedded Xinu, perhaps the main question is why USB devices
and/or the USB controller do not, by default, show up as device(s) in
``devtab`` like other Embedded Xinu devices. The reasons are that USB is
a dynamic bus, so it cannot be described by a static table, and also
because the highly nested structure of USB devices, as well as
multiple supported transfer types, is too complicated for the simple
":source:`read() <system/read.c>` and :source:`write()
<system/write.c>` from a device" paradigm.

.. note::
    Specific USB device drivers can still provide device entries in
    ``devtab`` if needed.  However, they must account for the fact
    that the physical devices are still hot-pluggable.

.. note::

    Not all Embedded Xinu :ref:`platforms <supported_platforms>`
    support USB, either due to not having USB hardware available or
    not having an appropriate USB host controller driver implemented.

.. _usb_components:

Components
~~~~~~~~~~

-  The **USB Host Controller Driver** is responsible for actually
   sending and receiving data over the USB by making use of the
   platform-dependent host controller hardware. The purpose of this
   driver is to isolate differences in USB host controllers from all
   other code dealing with USB.  In Embedded Xinu, USB Host Controller
   Drivers must implement the interface declared in
   :source:`include/usb_hcdi.h`.  (However, as of this writing, there
   is only one Host USB Controller Driver implemented and it controls
   the :doc:`/arm/rpi/Synopsys-USB-Controller` used on the
   :doc:`/arm/rpi/Raspberry-Pi`.)
-  The **USB Core Driver** is responsible for maintaining the USB device
   model, including the tree structure, and providing a framework in
   which USB device drivers can be written. It provides many convenience
   functions that simplify USB device driver development over using the
   Host Controller Driver directly; this can be viewed as an attempt to
   isolate the platform-dependent Host Controller Driver as much as
   possible. It also handles configuration that is common to all USB
   devices, such as setting a device configuration and address, and
   reading descriptors. In Embedded Xinu, the USB Core Driver can be
   found in :source:`device/usb/usbcore.c`.
-  **USB device drivers** are responsible for controlling specific USB
   devices. Since USB is a dynamic bus, USB device drivers are bound to
   actual USB devices at runtime with the help of USB Core Driver. A
   very important USB device driver that must always be implemented in
   any USB software stack is the **USB hub driver**, which is
   responsible for monitoring the status of a USB hub and reporting to
   the USB Core Driver when devices have been attached or detached.
   Embedded Xinu's USB hub driver can be found in
   :source:`device/usb/usbhub.c`. Other USB device
   drivers can be found in :source:`device/`; e.g.
   :source:`device/smsc9512/`.

.. note:: More complete (and complicated) USB software stacks, such as
          Linux's, also support **USB interface drivers**, which are associated
          with USB interfaces rather than USB devices.

Enabling Embedded Xinu's USB Support
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

To include support for USB in a given build of Embedded Xinu, define
``WITH_USB`` in ``xinu.conf`` and add ``usb`` to the ``DEVICES``
variable in ``platformVars``.  Note that the USB hub driver will be
included automatically as it is required for USB to support any
devices whatsoever.

In addition, you need to ensure that an appropriate Host Controller
Driver, which is platform-dependent code and is not located in this
directory, has been written and is compiled into the kernel.  For
example, :source:`system/platforms/arm-rpi/usb_dwc_hcd.c` is the Host
Controller Driver that is used on the Raspberry Pi hardware.

Finally, you need to enable any actual USB devices you want to support
by adding the corresponding device directories to the ``DEVICES``
variable in ``platformVars``, then defining the appropriate static
devices in ``xinu.conf``.  For example, on the Raspberry Pi, we enable
the driver for the SMSC LAN9512 USB Ethernet Adapter, which is located
in :source:`device/smsc9512`, by adding ``smsc9512`` to ``DEVICES``
and defining the ``ETH0`` device in ``xinu.conf``.

USB for embedded systems
~~~~~~~~~~~~~~~~~~~~~~~~

For fully embedded systems where debugging facilities are not
critical, unnecessary human-friendly functionality can be omitted from
the USB core.  See :source:`device/usb/usbdebug.c` for more details.

USB-related shell commands
~~~~~~~~~~~~~~~~~~~~~~~~~~

The **usbinfo** :doc:`shell command <Shell>` prints out information
about devices attached to the USB.  See :source:`shell/xsh_usbinfo.c`
for more details, or run ``usbinfo --help``.

.. _how_to_write_usb_device_driver:

How to write a USB device driver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You first of all must acquire any available documentation for the USB
device.  Note that many devices do not have their own documentation
because they conform to one of the USB *class specifications*; in such
cases the documentation is the class specification, even though these
are typically fairly long and complicated.

For nonstandard devices with no documentation available, you will have
to use whatever means are available to you for understanding the
device protocol, such as source code for other operating systems.  As
a last resort, the software interface to a USB device can be
reverse-engineered by snooping on USB traffic generated by binary
drivers.

Either way, to write the driver you will need to understand the format
and meaning of messages sent to and from the device, and which USB
endpoints and transfer types they are associated with.

Examples:

- USB Human Interface Devices such as mice are required to have an IN
  interrupt endpoint which is used to report input data such as mouse
  coordinates, and certain metadata can be queried from the default
  control endpoint.
- USB networking devices, such as the :doc:`/arm/rpi/SMSC-LAN9512`,
  provide a bulk IN endpoint for receiving networking packets and a
  bulk OUT endpoint for sending network packets.

On to the code itself:  In Embedded Xinu, USB device drivers are
implemented using the API provided by the USB Core Driver, which is
declared in :source:`usb_core_driver.h`.  This API allows drivers to
register themselves, bind themselves to USB devices that are detected
by the core, and communicate with USB devices.  It is documented
fairly extensively in the source; also see :source:`device/smsc9512/`
for an example of a USB device driver.

Note that Xinu's static device model is incompatible with USB's
dynamic device model, which is something that needs to be worked
around by the USB device drivers.  For example, the driver might
refuse to bind itself to more than a fixed number of USB devices, and
it might block or return failure if code tries to open the static
device before it has actually been bound to an actual USB device.

How to write a USB host controller driver
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In Embedded Xinu, the USB Host Controller Driver is responsible for
actually interacting with the hardware (the USB Host Controller) to
send and receive data over the USB.  Unfortunately, USB Host
Controllers are not standardized by the USB specification itself,
which is why this abstraction layer is needed.  USB Host Controllers
include those compliant with the UHCI, OHCI, or EHCI specifications,
as well as nonstandard ones such as the
:doc:`/arm/rpi/Synopsys-USB-Controller` used on the
:doc:`/arm/rpi/Raspberry-Pi`.

The very first step is to determine whether Xinu already supports the USB Host
Controller for the hardware under consideration.  If so, you can use that code,
but some changes may be needed (e.g. the location of memory-mapped registers).
Otherwise, read on....

The USB Host Controller Driver must implement the interface declared in
:source:`include/usb_hcdi.h`.

You first must acquire any documentation (if it exists) for the Host
Controller.  You also need to read relevant parts of the USB 2.0
Specification, mainly those that describe control, interrupt, and bulk
transfers.  Most of the 650 pages you do **not** need to read.

Next, in ``hcd_start()``, you must write any code that is needed to prepare the
Host Controller to be ready to use.

The next and essentially final step is to implement
``hcd_submit_xfer_request()``, which is very difficult.  You should initially
focus on faking requests that are sent to the root hub.  These will include
various control transfers to and from the root hub's default endpoint as well
as an interrupt transfer from the root hub's status change endpoint.  Some
root hub requests can be handled entirely in software; others will need to
communicate with the Host Controller.  Next, you must support control
transfers to and from actual USB devices on the bus.  Finally, you must
support interrupt and bulk transfers.  These must be asynchronous and
interrupt-driven.  Note that the hub driver uses interrupt transfers in order
to detect port status changes; thus, it will be impossible to enumerate the
entire USB until interrupt transfers have been implemented.

You can use the ``usb_debug()`` and ``usb_dev_debug()`` macros to
print debugging messages.  Enable them by changing the logging
priorities in :source:`include/usb_util.h`.

Further reading
---------------

- `USB 2.0 Specification <http://www.usb.org/developers/docs/>`__
- `USB 3.1 Specification <http://www.usb.org/developers/docs/>`__
- Embedded Xinu USB 2.0 subsystem. (:source:`device/usb`)
- Embedded Xinu USB device drivers. (Example: :source:`device/smsc9512/`)
- Embedded Xinu USB host controller drivers. (Example: :source:`system/platforms/arm-rpi/usb_dwc_hcd.c`)

