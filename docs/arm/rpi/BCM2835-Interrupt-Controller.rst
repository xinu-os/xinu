BCM2835 Interrupt Controller
============================

The **BCM2835 Interrupt Controller** is a memory-mapped peripheral
available on the :doc:`BCM2835` used in the :doc:`Raspberry-Pi`.  It
allows software to enable or disable specific IRQs (interrupt
requests).  Each IRQ usually corresponds to some sort of device
available on the chip.

Hardware Details
----------------

It is important to understand that on the BCM2835, some IRQs are
shared between the ARM CPU and VideoCore GPU.  This interrupt
controller controls **both** these shared IRQs as well as a few
ARM-specific IRQs, and the layout of the registers reflects this
separation.  Some of the shared IRQs are already enabled by the GPU
and therefore should not be enabled.  However, this interrupt
controller is only used by the ARM to control which interrupts
actually get routed to the ARM; the GPU most likely has its own
interrupt controller.

The BCM2835 Interrupt Controller is a memory-mapped peripheral
available at physical memory address 0x2000B000.  The following table
describes the registers, each of which is 32 bits.  **Note that the
offsets start at 0x200, not 0.  We do this for consistency with
Broadcom's documentation.  To be completely clear, IRQ_basic_pending
is located at physical memory address 0x2000B200.**


.. list-table:: BCM2835 Interrupt Controller registers
    :widths: 5 15 30
    :header-rows: 1

    * - Offset
      - Name
      - Description
    * - ``+0x200``
      - IRQ_basic_pending
      - Bitmask of pending ARM-specific IRQs, as well as additional
        bits (not currently used by Embedded Xinu) to accelerate
        interrupt handling.
    * - ``+0x204``
      - IRQ_pending_1
      - Bitmask of pending shared IRQs 0-31
    * - ``+0x208``
      - IRQ_pending_2
      - Bitmask of pending shared IRQs 32-63
    * - ``+0x20C``
      - FIQ_control
      - TODO
    * - ``+0x210``
      - Enable_IRQs_1
      - Write 1 to the corresponding bit(s) to enable one or more
        shared IRQs in the range 0-31
    * - ``+0x214``
      - Enable_IRQs_2
      - Write 1 to the corresponding bit(s) to enable one or more
        shared IRQs in the range 32-63
    * - ``+0x218``
      - Enable_Basic_IRQs
      - Write 1 to the corresponding bit(s) to enable one or more
        ARM-specific IRQs
    * - ``+0x21C``
      - Disable_IRQs_1
      - Write 1 to the corresponding bit(s) to disable one or more
        shared IRQs in the range 0-31
    * - ``+0x220``
      - Disable_IRQs_2
      - Write 1 to the corresponding bit(s) to disable one or more
        shared IRQs in the range 32-63
    * - ``+0x224``
      - Disable_Basic_IRQs
      - Write 1 to the corresponding bit(s) to disable one or more
        ARM-specific IRQs

For this interrupt controller to actually be of any use, the mapping
of IRQs to devices must be known.  The following table is an
**incomplete** list that documents the IRQs we have tested in our work
with Embedded Xinu.  The full list can be found `declared in a Linux
header
<https://github.com/raspberrypi/linux/blob/rpi-3.6.y/arch/arm/mach-bcm2708/include/mach/platform.h>`__
Below we use the numbering scheme used by both Embedded Xinu and
Linux, where the shared IRQs are numbered 0-63, and ARM-specific IRQs
are numbered starting at 64.

.. list-table:: Incomplete list of BCM2835 IRQs
    :widths: 5 15 30
    :header-rows: 1

    * - IRQ
      - Device
      - Notes
    * - 0
      - System Timer Compare Register 0
      - Do **not** enable this IRQ; it's already used by the GPU.
    * - 1
      - System Timer Compare Register 1
      - See :doc:`BCM2835-System-Timer`
    * - 2
      - System Timer Compare Register 2
      - Do **not** enable this IRQ; it's already used by the GPU.
    * - 3
      - System Timer Compare Register 3
      - See :doc:`BCM2835-System-Timer`
    * - 9
      - USB Controller
      - This is the *only* USB IRQ because all communication with USB
        devices happens through the USB Controller.  See
        :doc:`Synopsys-USB-Controller`.
    * - 55
      - PCM Audio
      -
    * - 62
      - SD Host Controller
      -

Notes:

- Software cannot "clear" interrupts using the interrupt controller.
  Instead, interrupts must be cleared in a device-specific way.

- Although some shared interrupts appear in the IRQ_Basic_Pending
  register as well as in the IRQ_Pending_1 or IRQ_Pending_2 registers,
  they cannot be enabled or disabled in Enable_Basic_IRQs or
  Disable_Basic_IRQs.

Use in Embedded Xinu
--------------------

Embedded Xinu (that, is :doc:`XinuPi`) uses the BCM2835 Interrupt
Controller to implement ``enable_irq()`` and ``disable_irq()``.  The
code is in :source:`system/platforms/arm-rpi/dispatch.c`.  These
functions simply are passed the number of the IRQ to enable or
disable.  Shared IRQs are numbered 0-63, while ARM-specific IRQs
(currently not actually used) are numbered starting at 64.  Also in
this file you will find ``dispatch()``, which is called from the
assembly language IRQ handler in
:source:`system/arch/arm/irq_handler.S` to handle an interrupt.  The
purpose of ``dispatch()`` is to figure out which number IRQs are
actually pending, then call the registered interrupt handler for each.

External Links
--------------

- `BCM2835 ARM Peripherals datasheet by Broadcom
  <http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf>`__
  The interrupt controller is documented in Section 7 (p. 109-118).
  Compared to some of the Raspberry Pi hardware, this is one of the
  better documented components.  Beware, though, that Broadcom's docs
  don't mention some of the important IRQ numbers, such as 0-3 (System
  Timer) and 9 (USB Controller).
