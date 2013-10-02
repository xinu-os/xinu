Assignment: Priority Scheduling and Process Termination
=======================================================

This assignment is a Xinu assignment allowing the student to grow a more
firm understanding of hhow the basics of an operating system work. This
assignment is part of the `Student Built Xinu <Student Built Xinu>`__
track for professors that are `Teaching With
Xinu <Teaching With Xinu>`__. This assignment should be completed in
teams of two. Students should only turn in the files required for this
assignment: ``system/create.c``, ``system/ctxsw.S``, and a ``README``
file, as explained below. The entire directory containing the operating
system should be turned in during the submission process.

Preparation
-----------

First, make a fresh copy of your work thus far. cp -R Untar the new
project files on top of this new directory: tar xvzf You should now see
the new project files in with your old files. Be certain to make clean
before compiling for the first time.

Priority Scheduling
-------------------

Add priority scheduling to your operating system. This can be done in 5
easy steps:

-  Add a priority field into PCB structure defined in
   ``include/proc.h``.
-  Add an effective priority field "``key``\ " into the process queue
   structure defined in ``include/queue.h``. This effective priority
   field will be used as a key to sort the queue.
-  Add priority parameter to ``create()``, and properly initialize the
   priority field in each new PCB created.
-  Build a new function in ``system/prioritize.c`` to implement a
   priority sorted ready queue. This does not require making any
   modifications to the primary queue maintenance functions already in
   ``system/queue.c``. Add your new source file in ``compile/Makefile``.
-  Modify the ``ready()`` and ``resched()`` functions to properly use
   your new priority scheduler.

New testcases are in ``system/main.c``, which should demonstrate
priority-order execution once your new scheduler is operational. You
will need to create others to fully test your implementation.

Starvation and Aging
--------------------

One of the chief drawbacks to simple priority scheduling is that low
priority processes may be *starved* by high priority processes, that is,
they may never get to run at all. One remedy for this it to implement
aging, a scheme in which the effective priority of a process increases
the longer it sits in the ready list without running.

Add a kernel configuration parameter "``AGING``\ " into
``include/kernel.h`` that when ``TRUE`` causes the effective priority of
each process in the ready list to increase by one every time
``resched()`` is called.

Construct a test case that demonstrates process starvation when
``AGING`` is ``FALSE``, but demonstrates aging when ``AGING`` is set to
``TRUE``. Call your test case function main-starve, and put it in your
``main.c``.

Preemption
----------

The new files ``system/clock*`` (as well as some adjustments to various
loader configuration files) will provide you with basic preemption, as
discussed in class. Take time to familiarize yourself with the contents
of these files, as you will be responsible for understanding how these
components of the operating system work.

Activate preemption by changing the ``PREEMPT`` constant in
``include/kernel.h`` to ``TRUE``. How can you test that preemption is
working in your system? Create a main program called ``main-preempt``
that demonstrates preemptive scheduling. Add it to ``main.c``.

Notes
-----

-  To submit your project, please run ``"make clean"`` in your compile
   directory, change directory back two levels, and submit the entire
   system via the professor's turn in instructions.
-  To complete this assignment, you will have made changes to several
   header files in the ``include/`` directory, and half the .c files in
   the ``system/`` directory.
-  Your professor may set up groups to allow easier collaboration
   between partners. If groups have been provided the permissions on
   your files should be modified to allow access to the group that
   includes both you and your partner.
-  When collaborating on a project in a shared directory, good software
   practice entails revision control, a few options include Subversion,
   CVS, and RCS. Using a revision control system can be very helpful and
   times, so you may wish to determine which systems are available in
   your lab.

`Category:Student Built Xinu <Category:Student Built Xinu>`__
