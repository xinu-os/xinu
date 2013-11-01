XinuPi
======

**XinuPi** is the port of Embedded Xinu to the :doc:`Raspberry-Pi`.
XinuPi provides a simple, lightweight operating system for the
Raspberry Pi that contains several orders of magnitude fewer lines of
code than the Linux-based software stacks that normally run on the
device. Its purpose is to provide an inexpensive, convenient platform
for various areas of computer science education at a University level,
including operating systems, embedded systems, networking, and
programming languages. Another goal of XinuPi is to document some of
the Raspberry Pi's hardware that has, until this point, been poorly
documented or even undocumented. This includes the documentation here
as well as XinuPi source code and the documentation generated from
comments in it.

Acquiring the Raspberry Pi hardware
-----------------------------------

See :doc:`Raspberry-Pi` for information about the hardware itself.

.. _xinupi_getting_started:

Downloading, compiling, and running XinuPi
------------------------------------------

Information about downloading and compiling Embedded Xinu can be found
in :doc:`/Getting-Started`.  To compile for the Raspberry Pi platform
(that is, to build "XinuPi"), you will need to set
``PLATFORM=arm-rpi`` when you run **make**.  You will also need an ARM
cross compiler (i.e.  binutils and gcc built with
``--target=arm-none-eabi``); more information can be found in
:ref:`cross_compiler`.

Note: currently, the Raspberry Pi support is only present in the
development version (from git) and not any released tarballs.

The compilation process will produce a file ``compile/xinu.boot``,
which can be copied to ``kernel.img`` on the SD card of a Raspberry Pi
to run it (see :ref:`raspberry_pi_booting`).

.. _xinupi_features:

XinuPi features and implementation
----------------------------------

-  The core of XinuPi provides a preemptive multitasking operating
   system for the Raspberry Pi. See
   :doc:`/arm/ARM-Preemptive-Multitasking` for more details about how
   Embedded Xinu implements preemptive multitasking on ARM-based
   platforms such as the Raspberry Pi; this includes information about
   thread creation and context switching.  Also see
   :doc:`BCM2835-System-Timer` for the timer on the Raspberry Pi that
   XinuPi uses to implement preemptive multitasking.

-  Interrupt handling on the Raspberry Pi, required for the timer
   interrupt as well as many other devices, is described in
   :doc:`Raspberry-Pi-Interrupt-Handling`.

-  USB support was added to Embedded Xinu partly because of its
   important role in the Raspberry Pi, including to attach the Ethernet
   Controller on the Raspberry Pi Model B. See USB for general
   information about USB, or :doc:`Synopsys-USB-Controller` for
   information specifically about the USB controller the Raspberry Pi
   provides.

-  See :doc:`SMSC-LAN9512` for information about the built-in USB
   Ethernet Adapter on the Raspberry Pi Model B, and XinuPi's driver
   for it.
