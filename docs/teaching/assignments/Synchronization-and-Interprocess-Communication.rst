Assignment: Synchronization and Interprocess Communication
==========================================================

.. epigraph::

   Neither snow nor rain nor heat nor gloom of night stays these couriers from
   the swift completion of their appointed rounds.

   -- USPS Unofficial Slogan

This assignment is a Xinu assignment allowing the student to grow a
more firm understanding of how an operating system works. This
assignment is part of the :doc:`/teaching/Student-Built-Xinu` track for
professors that are :doc:`teaching with Xinu </teaching/index>`. Written answers
to the analysis questions should be put into a file called
``system/ANALYSIS.txt`` and submitted with the code. The entire
directory containing the operating system should be turned in during
the submission process.

After completion of this assignment we will have a basic operating
system with preemptive, priority scheduling of processes, and counting
semaphores with wait queues as synchronization primitives and mailboxes
for interprocess communication.

Atomicity
---------

In a system with preemption, we must now guard against being interrupted
while in the middle of an atomic operation. There are many points in
operating system code where an unfortunately timed interrupt could leave
the system in an inconsistent state.

For example, in the ``resched()`` function, there are several lines of C
code (which translate into many more lines of machine code) between the
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

Preparation
-----------

First, make a fresh copy of your work thus far::

    cp -R

Untar the new project files on top of this new directory::

    tar xvzf

You should now see the new project files in with your old files. Be
certain to make clean before compiling for the first time.

Semaphores
----------

With this assignment you must learn about and understand classic
semaphores before using them to implement the remainder of the
assignment. An implementation of classic semaphores with waiting queues
has been provided for you. Please examine and understand the
implementation which can be found across several files including
``include/semaphore.h``, ``system/newsem.c``, ``system/signal.c`` and
``system/wait.c``. As part of the analysis portion of this assignment,
you will need to write a main file to show your understanding of
semaphores.

Mailboxes
---------

Using the provided semaphore structure, implement mailboxes as a means
for interprocess communication. A structure outlining mailboxes can be
found in ``include/mailbox.h``.

For completion of this assignment, your operating system should have a
fully functioning mailbox system for interprocess communication. The
files for the mailbox system can be found in the mailbox directory,
including ``mailbox/mailboxAlloc.c``, ``mailbox/mailboxFree.c``,
``mailbox/mailboxInit.c``, ``mailbox/mailboxReceive.c`` and
``mailbox/mailboxSend.c``.

Analysis
--------

For the analysis portion of this assignment, you will write two main
files to show your understanding of the operating system. Previously,
you would copy and paste your code over the given ``system/main.c`` to
run it. From this point on, you should add the test files to your
Makefile (``compile/Makefile``) to allow them to compile. Then, in
``system/initialize.c``, edit the process created when the system is
initialized to use the test function instead of ``main()``.

-  Create a simple ``main`` program, with function called
   ``main-hw6-q1``, that has multiple processes that use a mutex locking
   semaphore to alter a shared resource. This program should be saved as
   ``system/main-hw6-q1.c`` and should run properly when
   ``system/initialize.c`` is altered to ``create()`` a process for
   ``main-hw6-q1()`` instead of ``main()``.
-  Create another ``main`` program called ``main-hw6-q2()``, that uses
   mailboxes for effective interprocess communication. This program
   should be saved as ``system/main-hw6-q2.c`` and should run properly
   when ``system/initialize.c`` is altered to ``create()`` a process for
   ``main-hw6-q2()`` instead of ``main()``.
-  Give a situation in which mailboxes could be used to model a
   real-world problem; store your answer as Q3 in
   ``system/ANALYSIS.txt``.
-  Give a situation that will deadlock the system using your mailbox
   implementation. Why does it deadlock? What additional things might be
   added to the mailbox system or the operating system in general to fix
   this deadlock situation? Store your answers as Q4 in
   ``system/ANALYSIS.txt``.
