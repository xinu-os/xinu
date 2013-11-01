Interrupt Handling (ARM)
========================

This page provides an overview of how Embedded Xinu performs interrupt
handling on `ARM architectures
<http://en.wikipedia.org/wiki/ARM_architecture>`__.  This page only
concerns ARM-specific details; in particular it must be understood
that the actual meaning prescribed to interrupts is determined using a
board-specific mechanism, such as the
:doc:`rpi/BCM2835-Interrupt-Controller` on the
:doc:`rpi/Raspberry-Pi`.  Furthermore, note that the ARM architecture
and its exception/interrupt handling mechanisms are well documented by
ARM Ltd., especially in various versions of the `ARM Architecture
Reference Manual <http://infocenter.arm.com/help/index.jsp>`__. This
page is only intended to give an overview of relevant details in the
context of Embedded Xinu.

.. contents::
   :local:

IRQs and FIQs
-------------

Overview
~~~~~~~~

ARM processors define two types of "interrupts":

-  **IRQs** (Interrupt Requests). These are the "normal" type of
   interrupt.
-  **FIQs** (Fast Interrupt Requests). These are an feature that
   software can optionally use to increase the speed and/or priority of
   interrupts from a specific source. For simplicity, Embedded Xinu does
   **not** use FIQs. However, FIQs could be useful for those looking to
   design real-time and embedded software on top of or instead of the
   base Embedded Xinu kernel.

Both IRQs and FIQs are examples of **exceptions** supported by the ARM.
Beware that the term "IRQ" is often used generically, whereas here it
specifically refers to the ARM-architecture IRQ exception.

Receiving an IRQ or FIQ
~~~~~~~~~~~~~~~~~~~~~~~

When the ARM receives an IRQ, it will enter a special **IRQ mode**
and, by default, begin execution at physical memory address ``0x18``.
Similarly, when the ARM receives a FIQ, it will enter a special **FIQ
mode** and, by default, begin execution at physical memory address
``0x1C``. Before enabling IRQs or FIQs, software is expected to copy
ARM instructions to the appropriate address. In the case of IRQs,
there is only room for one ARM instruction, so it needs to be a branch
instruction to a place where the full handler is stored. In Embedded
Xinu, these special "glue" instructions, or **exception vectors**, are
set in :source:`loader/platforms/arm-rpi/start.S`.  The "full" IRQ
handler is located in :source:`system/arch/arm/irq_handler.S`.

Banked registers
~~~~~~~~~~~~~~~~

In IRQ mode and FIQ modes, some registers are **banked**, meaning that
their contents are dependent on the current processor mode. The
advantage of such registers is that their original values do not need
to be explicitly saved by the interrupt handling code. FIQ mode banks
more registers than IRQ mode, but both IRQ mode and FIQ mode bank the
stack pointer (sp), which essentially means that each mode can use its
own stack. However, for simplicity and consistency with other CPU
architectures, Embedded Xinu does **not** use this capability.
Instead, the interrupt handling code immediately switches the
processor from IRQ mode to "System" mode, which is the mode in which
Embedded Xinu normally operates the ARM CPU. This means that the
interrupt handling code uses the stack of the currently executing
thread, so perhaps the main disadvantage of this approach is that it
increases the stack size required by each thread.

Managing interrupts
-------------------

The ARM responds to IRQs and FIQs if and only if bits 7 and 6,
respectively, of the Current Program Status Register (``cpsr``) are 0.
By default (after reset) these bits are both 1, so software must
initially set them to 0 to enable IRQs and FIQs. Similarly, software
can set them to 1 if it needs to disable IRQs and FIQs. However,
software does not necessarily need to explicitly manipulate these bits
because an alternate instruction named ``cps`` (Change Program State)
is available that can handle changing these bits, as well as changing
processor modes.

Below we explain the ``enable()``, ``disable()``, and ``restore()``
functions used by Embedded Xinu to manage interrupts. These are all
implemented in the ARM assembly language file
:source:`system/arch/arm/intutils.S`.

``enable()``
~~~~~~~~~~~~

``enable()`` allows the processor to receive IRQ exceptions:

.. code-block:: none

    enable:
            cpsie i
            mov pc, lr

``enable()`` executes the ``cpsie`` ("Change Program State Interrupt
Enable") instruction to enable IRQs. (Recall that FIQs are not used by
Embedded Xinu.) It then overwrites the program counter (``pc``) with
the link register (``lr``) to return from the function. Note that
since the second instruction is merely overhead of a function call,
``enable()`` could instead be efficiently implemented as an inline
function containing inline assembly.

``disable()``
~~~~~~~~~~~~~

``disable()`` blocks IRQ exceptions and returns a value that can be
passed to ``restore()`` to restore the previous state. The previous
state may be either IRQs disabled or IRQs enabled. Note that an IRQ
exception received during a region of code where interrupts are
``disable()``-d is not lost; instead, it remains pending until IRQs
are re-enabled.

.. code-block:: none

    disable:
            mrs r0, cpsr
            cpsid i
            mov pc, lr

``disable()`` copies the ``cpsr`` (Current Program Status Register)
into ``r0``, which as per the ARM calling convention [#calling]_ is
the return value of the function. Therefore, the ``cpsr`` is treated
as the value that can be passed to ``restore()`` to restore the
previous interrupt state. The code then executes the ``cpsid`` (Change
Program State Interrupt Disable) instruction to actually disable the
IRQ exception.

``restore()``
~~~~~~~~~~~~~

``restore()`` restores the IRQ exceptions disabled/enabled state to the
state before a previous call to ``disable()``.

.. code-block:: none

    restore:
            msr cpsr_c, r0
            mov pc, lr

As per the ARM calling convention [#calling]_, the argument to
``restore()`` (the previous state value--- in the code this is often
stored in a variable named ``im``, for "interrupt mask") is passed in
``r0``. ``r0`` is then copied to the ``cpsr`` (Current Program Status
Register), which is the opposite of what ``disable()`` does.
``restore()`` then overwrites the program counter with the link
register to return from the function. Note that since the second
instruction is merely overhead of a function call, ``restore()`` could
instead be efficiently implemented as an inline function containing
inline assembly.

Further reading
---------------

As mentioned in the introduction, this page deals with ARM-architecture
details only and therefore does not provide a full explanation of
interrupt handling on any specific platform, which typically requires
the use of some interrupt controller to actually assign meaning to IRQ
exceptions.

- The interrupt controller on the :doc:`rpi/Raspberry-Pi` is the
  :doc:`rpi/BCM2835-Interrupt-Controller`.

Notes
-----

.. [#calling] http://infocenter.arm.com/help/topic/com.arm.doc.ihi0042e/IHI0042E_aapcs.pdf
