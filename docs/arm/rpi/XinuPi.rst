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

.. contents::
   :local:

Acquiring hardware
------------------

See :doc:`Raspberry-Pi` for information about the hardware itself.

Downloading and compiling
-------------------------

XinuPi is currently only available in source code form and only in the
:doc:`Embedded Xinu git repository </development/Git-Repository>`. To obtain the
code, install `git <http://git-scm.com/>`__ and run:

.. code-block:: none

    git clone https://github.com/xinu-os/xinu
    cd xinu

Note that this in fact the code for "Embedded Xinu" and not for
"XinuPi" specifically. That is, the Raspberry Pi is one of several
platforms that Embedded Xinu supports (and builds targeting it are
referred to as "XinuPi").

To compile XinuPi, run::

     make -C compile PLATFORM=arm-rpi

``PLATFORM=arm-rpi`` is necessary to instruct the build system to
target the Raspberry Pi. Additional arguments can be passed to
**make**, such as the ``COMPILER_ROOT`` to specify the location of a
GCC compiler targeting ARM (defaults to "``arm-none-eabi-``", meaning
that no explicit setting is needed if "``arm-none-eabi-gcc``" and the
corresponding binutils are already on the shell ``$PATH``).

The compilation process produces a file ``compile/xinu.boot``, which
can be copied to ``kernel.img`` on the SD card of a Raspberry Pi to
run it (see :ref:`Raspberry_Pi#Booting <Raspberry-Pi#Booting>`).

XinuPi is licensed under a BSD-style license; see the copyright
information in the source distribution for more details.

Features and implementation
---------------------------

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
