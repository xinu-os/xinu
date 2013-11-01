BCM2835 System Timer
====================

The **BCM2835 System Timer** is a memory-mapped peripheral available
on the :doc:`BCM2835` used in the :doc:`Raspberry-Pi`.  It features a
64-bit free-running counter that runs at 1 MHz and four separate
"output compare registers" that can be used to schedule interrupts.
However, two output compare registers are already used by the
VideoCore GPU, leaving only two available for the ARM CPU to use.

Hardware details
----------------

The interface to the BCM2835 System Timer is a set of 32-bit
memory-mapped registers beginning at physical address ``0x20003000``.

.. list-table:: BCM2835 System Timer registers
    :widths: 10 10 40
    :header-rows: 1

    * - Offset
      - Name
      - Description
    * - ``+0x00``
      - ``CS``
      - System Timer Control and Status
    * - ``+0x04``
      - ``CLO``
      - System Timer Counter Lower 32 bits
    * - ``+0x08``
      - ``CHI``
      - System Timer Counter Upper 32 bits
    * - ``+0x0C``
      - ``C0``
      - System Timer Compare 0; corresponds to IRQ line 0.
    * - ``+0x10``
      - ``C1``
      - System Timer Compare 1; corresponds to IRQ line 1.
    * - ``+0x14``
      - ``C2``
      - System Timer Compare 2; corresponds to IRQ line 2.
    * - ``+0x18``
      - ``C3``
      - System Timer Compare 3; corresponds to IRQ line 3.

``CLO`` and ``CHI`` form a 64-bit free-running counter, which
increases by itself at a rate of 1 MHz, that software can read to get
the current number of timer ticks.  There are, however, two caveats:

- Appropriate :doc:`memory barriers <BCM2835-Memory-Barriers>` should
  be inserted to guarantee that read data is not re-ordered with that
  from a different peripheral.
- Reading ``CLO`` can be done in a single 32-bit access.  However, we
  are not currently aware of a way to read ``CLO`` and ``CHI``
  together atomically.  To work around this when the full 64-bit time
  is desired, software can read ``CHI``, then ``CLO``, then read
  ``CHI`` and retry if ``CHI`` changed.

To schedule an interrupt using the System Timer, software can write
the value of ``CLO`` at which an interrupt will be generated into one
of the System Timer Compare registers.  However, the CPU actually only
use ``C1`` and ``C3``, since ``C0`` and ``C2`` are used by the GPU.
Also, to actually receive the scheduled interrupt, the software must
have previously enabled the corresponding IRQ line using the
:doc:`BCM2835-Interrupt-Controller`.  To clear the interrupt, software
must write 1 to the bit in ``CS`` that has the index the same as that
of the System Timer Compare register.  That is, to clear an interrupt
set in ``C1``, software must write ``0x20`` to ``CS``, and to clear an
interrupt set in ``C3``, software must write ``0x80`` to ``CS``.

Use in Embedded Xinu
--------------------

In the :doc:`Raspberry-Pi` port of Embedded Xinu, or :doc:`XinuPi`,
the BCM2835 System Timer is used to implement :doc:`preemptive
multitasking </features/Preemptive-Multitasking>` and keep the system time.
The code can be found in :source:`system/platforms/arm-rpi/timer.c`
and is fairly simple, as it only needs to implement ``clkcount()`` and
``clkupdate()``.
