Assignment: Delta Queues
========================

This assignment is a Xinu assignment allowing the student to grow a
more firm understanding of how an operating system works. This
assignment is part of the :doc:`/teaching/Student-Built-Xinu` track for
professors that are :doc:`teaching with Xinu </teaching/index>`. Your entire
working directory containing your Xinu operating system will be
submission for this assignment.

Preparation
-----------

First, make a fresh copy of your work thus far::

 cp -R

Untar the new project files on top of this new directory::

 tar xvzf

You should now see the new project files in with your old files. Be
certain to make clean before compiling for the first time.

Sleep
-----

Modify ``system/sleep.c``, ``system/wakeup.c``, ``system/insertd.c``,
and ``system/clockintr.c`` to add the ``sleep()`` system call to the
operating system. A process that calls ``sleep(n)`` should enter the
``PRSLEEP`` state for n clock ticks. A clock "tick" is one millisecond
in our current system. It corresponds to the raising of the timer
interrupt request when interrupts are not disabled. When the clock
interrupt handler detects that a process has slept for long enough, the
``wakeup`` function should move the process back to the ready list.

The appropriate data structure for maintaining a list of sleeping
processes is a *delta queue*, as described in lecture. Add a delta queue
to the system by increasing the ``QENT`` constant in
``include/queue.h``, declaring a queue called "sleepq" in
``include/clock.h``, and and defining and initializing ``sleepq`` in
``system/clockinit.c``. Make sure to test your delta queue in isolation
before testing the ``sleep()`` function.

Grading
-------

Project credit will be weighted evenly between delta queue maintenance,
sleeping, and waking processes. By now, you should be aware that
rigorous testing is the key to success in these projects. Our test cases
will include at least:

-  Checking your delta queue insert function with a variety of
   parameters
-  Checking your sleeping and waking functions independently and in
   concert.
