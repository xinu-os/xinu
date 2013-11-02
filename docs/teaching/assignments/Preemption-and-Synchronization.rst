Assignment: Preemption and Synchronization
==========================================

This assignment is a Xinu assignment allowing the student to grow a
more firm understanding of how an operating system works. This
assignment is part of the :doc:`/teaching/Student-Built-Xinu` track for
professors that are :doc:`teaching with Xinu </teaching/index>`. Written answers
to the five analysis questions below should be put into a file called
``system/README`` and submitted with your code. The entire directory
containing the operating system should be turned in during the
submission process.

After completion of this assignment we will have a basic operating
system with preemptive, priority scheduling of processes, and counting
semaphores with wait queues as synchronization primitives. Explore how
the system behaves with various numbers of producers and consumers at
different levels of priority and with both blocking and non-blocking
signals. You will be responsible for making predictions about how such a
system behaves in subsequent projects. As an added challenge, you might
like to try implementing some of the other classic synchronization
problems outlined in the textbook.

Preparation
-----------

First, make a fresh copy of your work thus far::

  cp -R

Untar the new project files on top of this new directory::

  tar xvzf

You should now see the new project files in with your old files. Be
certain to make clean before compiling for the first time.

Preemption
----------

The new files ``system/clock*`` (as well as some adjustments to various
loader configuration files) will provide you with basic preemption, as
discussed in class. Take time to familiarize yourself with the contents
of these files, as you will be responsible for understanding how these
components of the operating system work.

How can you test that preemption is working in your system? Create a
main program that demonstrates preemptive scheduling. Call it
``main-preempt.c``, and make sure that it works when copied over
``main.c``.

Atomicity
---------

In a system with preemption, we must now guard against being interrupted
while in the middle of an atomic operation. There are many points in
operating system code where an unfortunately timed interrupt could leave
the system in an inconsistent state.

For example, in the ``resched()`` function, there are several lines of C
code (which translate into many more lines of machine code,) between the
point where the outgoing running process is put back into the queue of
ready processes and the point where an incoming process is dequeued and
set to running. If an interrupt were to take place in the midst of this
transition, the interrupt handling code might see the system in an
inconsistent state, in which there doesn't seem to be a currently
running process. For this reason, we consider process rescheduling to be
atomic with respect to the rest of the operating system. The easiest way
to enforce this is to temporarily disable interrupts during a critical,
atomic section of code, and then reenable them when done.

The new file ``system/intr.S`` contains functions ``enable()``,
``disable()``, and ``restore()`` for manipulating the master bit that
enables processor interrupts. From this point forward, these functions
must be used judiciously to guard atomic sections in the operating
system functions you write.

Semaphores
----------

Implement classic semaphores with waiting queues. A definition of the
basic semaphore structure is given in ``include/semaphore.h``, and is
initialized in the new ``system/initialize.c``.

Fill in the functions in ``system/newsem.c``, ``system/signal.c`` and
``system/wait.c``. A simple ``main.c`` is given that implements a
trivial case of the Producers/Consumers Problem.

Analysis
--------

-  Create a main program that spawns multiple producer processes (each
   producing a distinctive character output) and multiple consumer
   processes. What happens, and why? (Put your answers in
   ``system/README``.) Save this test case as ``system/main-q1.c``.
-  Using the same test case as in the previous question, briefly adjust
   the value of the preemption counter ("QUANTUM" in
   ``include/clock.h``) to be 100 mS. How does this change the output?
   (Put your answers in ``system/README``.)
-  Create a test case ``system/main-q3.c`` in which the consumer
   processes are all of a higher priority than the producer processes.
   Repeat the first two questions.
-  In the comment blocks for ``system/signal.c``, you were directed to
   reschedule after taking a waiting process off of the semaphore's wait
   queue. What happens if you call only ``ready()`` when taking a
   process off of the wait queue, and do not voluntarily yield at that
   time? (This is called non-blocking or non-yielding ``signal()``.)
   Repeat the first three questions with non-blocking ``signal()``.
-  One of the interesting things about the classic Producer/Consumer
   problem is how finely balanced the solution turns out to be. A
   variety of simple one-line swaps in either the producer code or the
   consumer code can produce a system of processes that readily
   deadlocks. Find one of these combinations, and save it as
   ``system/main-q5.c``
