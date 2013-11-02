Assignment: LL and SC
=====================

Synchronization and Interprocess Communication
----------------------------------------------

This assignment is a Xinu assignment allowing the student to grow a
more firm understanding of how an operating system works. This
assignment is part of the :doc:`/teaching/Student-Built-Xinu` track for
professors that are :doc:`teaching with Xinu </teaching/index>`. Written answers
to the analysis questions should be put into a file called
``system/ANALYSIS.txt`` and submitted with the code. The entire
directory containing the operating system should be turned in during
the submission process.

This assignment includes a LAB REPORT. Include your lab report in a file
called ``login.pdf``, (with your login name,) in the top-level directory
of your xinu-hw6.

After completion of this assignment we will have a basic operating
system with preemptive, priority scheduling of processes, counting
semaphores with wait queues as synchronization primitives and
interprocess communication with bounded buffers.

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
``include/semaphore.h``, ``system/newsem.c``, system/signal.c and
``system/wait.c``. As part of the analysis portion of this assignment,
you will need to write a main file to show your understanding of
semaphores.

Producers/Consumers
-------------------

Using the provided semaphore structure, implement producers and
consumers that communicate using a Bounded-Buffer. Your textbook
provides discussion of the Bounded-Buffer Problem beginning in
section 6.6.1, and outlines this assignment as
Programming Project 6.40 - Producer-Consumer Problem.

Segments of the textbook code are already given for you in
``system/testcases.c``. Complete the project as specified in the text
with slight adjustments as noted in the testcases TODOs. Answer the
analysis questions below in your lab report.

Synchronization Hardware
------------------------

Disabling all interrupts is an effective but heavy-handed approach for
providing mutual exclusion. Multicore systems and complex real-time
systems often cannot afford to disable interrupts, and rely more on
hardware support for atomic updates.

The assembly code in ``system/testAndSet.S`` implements the
``testAndSet()`` operation (ala Figure 6.4 in your textbook [1]_) using
the MIPS LL/SC (load-linked and store-conditional) opcodes. Much more
complex synchronization primitives can be built on top of simple atomic
opcode combinations such as there.

Build ``mutexAcquire()`` and ``mutexRelease()`` in ``system/mutex.c``
using the bounded-waiting algorithm presented in Figure 6.8 of your
textbook [1]_.

Lab Report
----------

For the lab report portion of this assignment, you should typeset a
document containing your analysis of the system components introduced. A
sample report format is given
`HERE <http://xinu/wiki/images/b/b6/Labreport.pdf>`__. We expect the
report to be written with your exemplary literacy skills.

What are we reporting on? Once you have the Producer/Consumer
implementation working with the provided semaphore API, consider the
following questions:

#. Does it matter which process runs first, the producer or the
   consumer?
#. What happens when there are multiple producers and consumers?
#. What happens when the producer(s) priority is higher than the
   consumer(s)? Vice-versa?

For each of these, form a hypothesis, construct an experiment
(testcases), and draw appropriate conclusions. There is no benefit in
fudging your hypotheses after you know the answer; the quality of a lab
report is orthogonal to whether your hypothesis was correct or not.

For part two of the assignment, use your bounded-wait ``mutexAcquire()``
and ``mutexRelease()`` to prevent your producers/consumers from breaking
up ``kprintf()`` calls mid-line without additional disabling of
interrupts. Consider the following questions:

#. Can your bounded-wait mutex subsystem replace the semaphore subsystem
   for this task?
#. Under what conditions in the embedded operating system will the mutex
   subsystem not work as designed?
#. Can you deadlock your producer and consumer?

I rewrote the ``testAndSet()`` MIPS code from lab today to correspond
more closely to the semantics used in the textbook chapter 6 [1]_. There
is an example of the `lab report
format <http://xinu/wiki/images/b/b6/Labreport.pdf>`__, and analysis
questions to report upon in the final section. Typeset your lab report
with whatever software you are comfortable with.

References
----------

.. [1]
   Silberschatz, A., Galvin, P. B., and Gagne, G. 2009 Operating System
   Concepts. 8th. John Wiley & Sons, Inc.
