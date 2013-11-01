Assignment: Priority Scheduling and Process Termination
=======================================================

This assignment is a Xinu assignment allowing the student to grow a
more firm understanding of how the basics of an operating system work.
This assignment is part of the :doc:`Student-Built-Xinu` track for
professors that are :doc:`teaching with Xinu <index>`.  This
assignment should be completed in teams of two. Students should only
turn in the files required for this assignment: ``system/create.c``,
``system/ctxsw.S``, and a ``README`` file, as explained below. The
entire directory containing the operating system should be turned in
during the submission process.

Preparation
-----------

First, make a fresh copy of your work thus far::

    cp -R
    
Untar the new project files on top of this new directory::

    tar xvzf

You should now see the new project files in with your old files. Be
certain to make clean before compiling for the first time.

Priority Scheduling
-------------------

Add priority scheduling to your operating system. This can be done in 5
easy steps:

-  Add a priority field into PCB structure defined in
   ``include/proc.h``.
-  Add an effective priority field into the process queue structure
   defined in ``include/queue.h``. This effective priority field will be
   used as a key to sort the queue.
-  Add priority parameter to ``create()``, and properly initialize the
   priority field in each new PCB created.
-  Complete the new function in ``system/insert.c`` to implement a
   priority sorted ready queue. This does not require making any
   modifications to the primary queue maintenance functions already in
   ``system/queue.c``.
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
*aging*, a scheme in which the effective priority of a process increases
the longer it sits in the ready list without running.

Add a kernel configuration parameter "AGING" into ``include/kernel.h``
that when ``TRUE`` causes the effective priority of each process in the
ready list to increase by one every time ``resched()`` is called.

Construct a testcase that demonstrates process starvation when ``AGING``
is ``FALSE``, but demonstrates aging when ``AGING`` is set to ``TRUE``.
Turn this testcase in as your ``main.c``.

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
