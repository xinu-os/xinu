Preemptive multitasking
=======================

Like virtually all modern operating systems, XINU supports
**preemptive multitasking**, which makes it appear that multiple
threads are executing at the same time on the same processor. Support
for preemptive multitasking consists of support for multiple threads
combined with a preemption mechanism.

.. _multiple_threads:
.. _thread_context:

Multiple threads
----------------

XINU supports multiple threads, but only one can execute at a time. A
**thread context** refers to the saved state of a thread, primarily
CPU registers. XINU platforms must implement two functions to allow
creating new threads and switching between threads using their thread
contexts:

-  ``setupStack()``, which is responsible for setting up the stack
   of a new thread to include an initial thread context and procedure
   arguments. This routine is typically implemented in C. It is called
   internally by ``create()``, located in :source:`system/create.c`.
   For an example, see :source:`system/arch/arm/setupStack.c`.
-  ``ctxsw()``, which is responsible for switching between threads. More
   specifically, this routine must save the thread context of the
   current thread and restore the thread context of the new thread. This
   routine is always implemented in assembly language. For an example,
   see :source:`system/arch/arm/ctxsw.S`.

Since different CPU architectures use different registers and calling
conventions, the size and format of a thread context varies depending on
the CPU architecture. Note that since thread contexts are created in
both ``setupStack()`` and ``ctxsw()``, these two routines must
create contexts that are compatible, at least to the extent that
``ctxsw()`` can either start a new thread or resume an existing thread.

Other articles describe this in more detail for specific architectures:

- :doc:`/arm/ARM-Preemptive-Multitasking`

.. _preemption:

Preemption
----------

Preemption occurs when the timer interrupt occurs and XINU attempts to
reschedule the currently executing thread, which results in a call to
``ctxsw()``, described above, that may switch to a different thread
context. (We say *may* because the code is written such that
``ctxsw()`` is called when the same thread is rescheduled to itself,
in which case ``ctxsw()`` restores the saved context immediately and
is a no-op.) The means of generating a timer interrupt is
platform-dependent and may even differ among platforms that share the
same CPU architecture. For an example, see
:doc:`/arm/rpi/BCM2835-System-Timer`, which is used in the
:doc:`/arm/rpi/Raspberry-Pi`, an ARM-based platform.
