XinuPi
======

**XinuPi** is the port of `Embedded Xinu <Embedded Xinu>`__ to the
`Raspberry Pi <Raspberry Pi>`__. XinuPi provides a simple, lightweight
operating system for the Raspberry Pi that contains several orders of
magnitude fewer lines of code than the Linux-based software stacks that
normally run on the device. Its purpose is to provide an inexpensive,
convenient platform for various areas of computer science education at a
University level, including operating systems, embedded systems,
networking, and programming languages. Another goal of XinuPi is to
document some of the Raspberry Pi's hardware that has, until this point,
been poorly documented or even undocumented. This includes the
documentation on this Wiki as well as the XinuPi source code and the
documentation generated from comments in it.

Acquiring hardware
------------------

See `Raspberry Pi <Raspberry Pi>`__ for information about the hardware
itself.

Downloading and compiling
-------------------------

XinuPi is currently only available in source code form and only in the
`Embedded Xinu git repository <git>`__. To obtain the code, install
`git <http://git-scm.com/>`__ and run:

| ``git clone ``\ ```https://github.com/xinu-os/xinu`` <https://github.com/xinu-os/xinu>`__
| ``cd xinu``

Note that this in fact the code for "Embedded Xinu" and not for "XinuPi"
specifically. That is, the Raspberry Pi is one of several platforms that
Embedded Xinu supports (and builds targeting it are referred to as
"XinuPi").

To compile XinuPi, run:

``make -C compile PLATFORM=arm-rpi``

PLATFORM=arm-rpi is necessary to instruct the build system to target the
Raspberry Pi. Additional arguments can be passed to *make*, such as the
COMPILER\_ROOT to specify the location of a GCC compiler targeting ARM
(defaults to "arm-none-eabi-", meaning that no explicit setting is
needed if "arm-none-eabi-gcc" and the corresponding binutils are already
on the shell $PATH). See for more details; for example, details about
cross compilers.

The compilation process produces a file "compile/xinu.boot", which can
be copied to "kernel.img" on the SD card of a Raspberry Pi to run it
(see `Raspberry Pi#Booting <Raspberry Pi#Booting>`__).

XinuPi is licensed under a BSD-style license; see the copyright
information in the source distribution for more details.

Features and implementation
---------------------------

-  The core of XinuPi provides a preemptive multitasking operating
   system for the Raspberry Pi. See `Preemptive multitasking
   (ARM) <Preemptive multitasking (ARM)>`__ for more details about how
   Embedded Xinu implements preemptive multitasking on ARM-based
   platforms such as the Raspberry Pi; this includes information about
   thread creation and context switching. Also see `BCM2835 System
   Timer <BCM2835 System Timer>`__ for the timer on the Raspberry Pi
   that XinuPi uses to implement preemptive multitasking.
-  Interrupt handling on the Raspberry Pi, required for the timer
   interrupt as well as many other devices, is described in `Interrupt
   handling (Raspberry Pi) <Interrupt handling (Raspberry Pi)>`__.
-  USB support was added to Embedded Xinu partly because of its
   important role in the Raspberry Pi, including to attach the Ethernet
   Controller on the Raspberry Pi Model B. See USB for general
   information about USB, or `Synopsys DesignWare High-Speed USB 2.0
   On-The-Go
   Controller <Synopsys DesignWare High-Speed USB 2.0 On-The-Go Controller>`__
   for information specifically about the USB controller the Raspberry
   Pi provides.
-  See `SMSC LAN9512 <SMSC LAN9512>`__ for information about the
   built-in USB Ethernet Adapter on the Raspberry Pi Model B, and
   XinuPi's driver for it.

TODO: sound, graphics, keyboard support
