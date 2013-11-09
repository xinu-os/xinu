USB keyboard driver
===================

Support for **USB (Universal Serial Bus) keyboards** has been added to
|EX| via the *usbkbd* device driver, located in
:source:`device/usbkbd/`.

The *usbkbd* driver is a :ref:`USB device driver <usb_components>` and
can only be built when Embedded Xinu's :doc:`USB <USB>` support has
been enabled.

External interface
------------------

Like other Embedded Xinu device drivers such as the various UART
drivers, the *usbkbd* driver provides one or more device table
entries, such as ``USBKBD0``, ``USBKBD1``, etc.  Characters can be
read from these devices simply by using :source:`getc()
<system/getc.c>` or :source:`read() <system/read.c>` on these devices.
These functions will block if the corresponding USB keyboard is
detached or has yet to be attached.

Scope
-----

The *usbkbd* driver only supports the keyboard boot protocol defined
in the USB HID (Human Interface Device) specification.  This protocol
defines a fixed 8-byte report that the keyboard provides over its IN
interrupt endpoint when a key is pressed or released.  *usbkbd* does
not parse the HID report descriptor and does not support any other
Human Interface Devices, such as mice (or steering wheels, or data
gloves...).

Since data is provided by *usbkbd* as 8-bit characters, there is
limited support for special keys that do not correspond to printing
characters, and currently most are simply ignored.

Implementation details
----------------------

When :source:`sysinit() <system/initialize.c>` runs, ``init()`` is
called on each device table entry, resulting in a call to
:source:`usbKbdInit() <device/usbkbd/usbKbdInit.c>` for each keyboard
device entry.  As soon as one of these calls occurs, *usbkbd*
registers itself with the :ref:`USB subsystem <usb_subsystem>` as a
USB device driver.  :source:`usbKbdInit()
<device/usbkbd/usbKbdInit.c>` does *not* wait for a keyboard to
actually be attached before returning.

When a USB device is attached, :source:`usbKbdBindDevice()
<device/usbkbd/usbKbdBindDevice.c>` is called by the :ref:`USB
subsystem <usb_subsystem>`.  If the device has a HID interface
supporting the keyboard boot protocol (i.e. "is a USB keyboard"), a
recurring USB transfer from the keyboard's IN interrupt endpoint is
started.  When each such transfer completes,
:source:`usbKbdInterrupt() <device/usbkbd/usbKbdInterrupt.c>` is
called by the :ref:`USB subsystem <usb_subsystem>`, and any new
characters detected are placed in a buffer for later retrieval by
reads from the keyboard device.

See also
--------

- :doc:`USB`
- :ref:`how_to_write_usb_device_driver`
- :doc:`TTY-Driver`

References
----------

- `Universal Serial Bus (USB) Device Class Definition for Human Interface Devices (HID), Version 1.11
  <http://www.usb.org/developers/devclass_docs/HID1_11.pdf>`__
- `Universal Serial Bus (USB) HID Usage Tables, Version 1.11
  <http://www.usb.org/developers/devclass_docs/Hut1_11.pdf>`__
