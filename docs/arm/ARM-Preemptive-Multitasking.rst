Preemptive multitasking (ARM)
=============================

This page documents how Embedded Xinu implements :doc:`preemptive
multitasking </features/Preemptive-Multitasking>` on :wikipedia:`ARM
architecture` platforms that it has been ported to, such as the
:doc:`rpi/Raspberry-Pi`.

Thread context
--------------

The format and size of the :ref:`thread context
<Preemptive-Multitasking#Multiple-threads>` used for ARM ports of
Embedded Xinu rests on two factors: the registers available on the ARM
architecture, and the standard ARM calling convention [#calling]_.

-  ARM processors have 16 "general-purpose" registers numbered ``r0`` - ``r15``,
   as well as the Current Program Status Register (``cpsr``). However,
   despite being considered "general-purpose" registers, ``r13`` - ``r15``
   actually have special purposes; namely, ``r13`` is used as the stack
   pointer (``sp``), ``r14`` is used as the link register (``lr``), and ``r15`` is used
   as the program counter (``pc``).

-  The standard ARM calling convention specifies which registers are
   callee-save (``r4`` - ``r11``, ``r13`` - ``r14``) and which are
   caller-save (``r0`` - ``r3``, ``r11``), as well as how arguments are passed to
   procedures (up to four arguments in ``r0`` - ``r3``; additional
   arguments spill onto stack).

The thread context we chose is 15 words long and is the following:

========     ========    ===========
Offset       Register    Notes
========     ========    ===========
``0x00``     ``r0``      First thread argument, caller-save
``0x04``     ``r1``      Second thread argument, caller-save
``0x08``     ``r2``      Third thread argument, caller-save
``0x0C``     ``r3``      Fourth thread argument, caller-save
``0x10``     ``r4``      Callee-save
``0x14``     ``r5``      Callee-save
``0x18``     ``r6``      Callee-save
``0x1C``     ``r7``      Callee-save
``0x20``     ``r8``      Callee-save
``0x24``     ``r9``      Callee-save
``0x28``     ``r10``     Callee-save
``0x2C``     ``r11``     Callee-save
``0x30``     ``cpsr``    Current program status register
``0x34``     ``lr``      Link register
``0x38``     ``pc``      Program counter
========     ========    ===========

The ARM version of ``ctxsw()`` is implemented in
:source:`system/arch/arm/ctxsw.S` and contains a detailed explanation
that will not be replicated here.

Preemption
----------

The means of generating a timer interrupt for :ref:`preemption
<Preemptive-Multitasking#Preemption>` is not standard to the ARM
architecture; instead, software must make use of a board-specific or
chip-specific device such as the :doc:`rpi/BCM2835-System-Timer`.

Notes
-----

.. [#calling] http://infocenter.arm.com/help/topic/com.arm.doc.ihi0042e/IHI0042E_aapcs.pdf
