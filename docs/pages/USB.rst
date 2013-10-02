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

Bus Topology
------------

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
-------

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
----------------

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
present standard interfaces (such as UHCI, OHCI, or EHCI--- all defined
in specifications separate from the USB specification itself) to
software. Others do not present a standard interface, but instead have
vendor-provided documentation and/or a vendor-provided driver; an
example of this is the `Synopsys DesignWare High-Speed USB 2.0 On-The-Go
Controller <Synopsys DesignWare High-Speed USB 2.0 On-The-Go Controller>`__
used on the `Raspberry Pi <Raspberry Pi>`__. Obviously, a standard
interface is highly preferred when independently implementing a Host
Controller Driver.

Transfers
---------

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
------

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

Software Architecture
---------------------

Now that some general information about USB has been presented, it
should be easier to understand the basic design of a USB software stack.
The description that follows is certainly not the only way to organize
the code, but it is the way that is used in most operating systems and
makes the most sense based on how USB was designed. In terms of Embedded
Xinu, perhaps the main question is why USB devices and/or the USB
controller do not show up as device(s) in 'devtab' like other Embedded
Xinu devices. The reasons are that USB is a dynamic bus, so it cannot be
described by a static table, and also because the highly nested
structure of USB devices, as well as multiple supported transfer types,
is too complicated for the simple "read() and write() from a device"
paradigm.

-  The **USB Host Controller Driver** is responsible for actually
   sending and receiving data over the USB by making use of the
   platform-dependent host controller hardware. The purpose of this
   driver is to isolate differences in USB host controllers from all
   other code dealing with USB. In Embedded Xinu, USB Host Controller
   Drivers must implement the interface declared in . However, as of
   this writing, there is only one Host USB Controller Driver
   implemented and it controls the `Synopsys DesignWare High-Speed USB
   2.0 On-The-Go
   Controller <Synopsys DesignWare High-Speed USB 2.0 On-The-Go Controller>`__
   used on the `Raspberry Pi <Raspberry Pi>`__).
-  The **USB Core Driver** is responsible for maintaining the USB device
   model, including the tree structure, and providing a framework in
   which USB device drivers can be written. It provides many convenience
   functions that simplify USB device driver development over using the
   Host Controller Driver directly; this can be viewed as an attempt to
   isolate the platform-dependent Host Controller Driver as much as
   possible. It also handles configuration that is common to all USB
   devices, such as setting a device configuration and address, and
   reading descriptors. In Embedded Xinu, the USB Core Driver can be
   found in .
-  **USB device drivers** are responsible for controlling specific USB
   devices. Since USB is a dynamic bus, USB device drivers are bound to
   actual USB devices at runtime with the help of USB Core Driver. A
   very important USB device driver that must always be implemented in
   any USB software stack is the **USB hub driver**, which is
   responsible for monitoring the status of a USB hub and reporting to
   the USB Core Driver when devices have been attached or detached.
   Embedded Xinu's USB hub driver can be found in . Other USB device
   drivers can be found in ; e.g. .

Note: more complete (and complicated) USB software stacks, such as
Linux's, also support **USB interface drivers**, which are associated
with USB interfaces rather than USB devices.

Further reading
---------------

-  USB 2.0 Specification. `1 <http://www.usb.org/developers/docs/>`__.
-  USB 3.1 Specification. `2 <http://www.usb.org/developers/docs/>`__.
-  Embedded Xinu USB 2.0 subsystem. (device/usb).
-  Embedded Xinu USB device drivers. (Example: ).
-  Embedded Xinu USB host controller drivers. (Example: ).

