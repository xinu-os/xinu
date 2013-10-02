Preemptive Multitasking (ARM)
=============================

This page documents how `Embedded Xinu <Embedded Xinu>`__ implements
`preemptive multitasking <Preemptive multitasking>`__ on
`ARM-architecture <http://en.wikipedia.org/wiki/ARM_Architecture>`__
platforms that it has been ported to, such as the `Raspberry
Pi <Raspberry Pi>`__.

Thread context
--------------

The format and size of the `thread
context <Preemptive_multitasking#Multiple_threads>`__ used for ARM ports
of Embedded Xinu rests on two factors: the registers available on the
ARM architecture, and the standard ARM calling
convention\ http://infocenter.arm.com/help/topic/com.arm.doc.ihi0042e/IHI0042E_aapcs.pdf</ref>:

-  ARM processors have 16 "general-purpose" registers numbered r0-r15,
   as well as the Current Program Status Register (cpsr). However,
   despite being considered "general-purpose" registers, r13-r15
   actually have special purposes; namely, r13 is used as the stack
   pointer (sp), r14 is used as the link register (lr), and r15 is used
   as the program counter (pc).

-  The standard ARM calling convention specifies which registers are
   callee-save (r4-r11, r13-r14) and which are caller-save (r0-r3, r11),
   as well as how arguments are passed to procedures (up to four
   arguments in r0-r3; additional arguments spill onto stack).

The thread context we chose is 15 words long and is the following:

TODO

The ARM version of ``ctxsw()`` is implemented in and contains a detailed
explanation that will not be replicated here.

Preemption
----------

The means of generating a timer interrupt for
`preemption <Preemptive_multitasking#Preemption>`__ is not standard to
the ARM architecture; instead, software must make use of a
board-specific or chip-specific device such as the `BCM2835 System
Timer <BCM2835 System Timer>`__.

Notes
-----

.. raw:: html

   <references />

