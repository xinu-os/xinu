Assignment: Context Switch and Non-Preemptive Scheduling
========================================================

This assignment is an early Xinu assignment allowing the student to
grow a more firm understanding of how the basics of an operating
system work.  This assignment is part of the :doc:`Student-Built-Xinu`
track for professors that are :doc:`teaching with Xinu <index>`. This
assignment should be completed in teams of two. Students should only
turn in the files required for this assignment: ``system/create.c``,
``system/ctxsw.S``, and a ``README`` file, as explained below.

Preparation
-----------

Untar the new project files in a fresh working directory::

  tar xvzf
  
Copy your synchronous serial driver file (``kprintf.c``) into the
``system/`` directory.

Context Switch
--------------

The new source files include two new headers, ``include/proc.h``
(definitions for Process Control Blocks and a process table), and
``include/queue.h`` (definition for a process ready queue), as well as
trivial updates to ``include/kernel.h``.

The new source files also include:

=======================      ===============================
``system/initialize.c``      Updated initialization for Project 4.
``system/main.c``            A "main program" for testing scheduling.
``system/queue.c``           An implementation of the queue data structure.
``system/create.c``          A partial function for creating a new process.
``system/ctxsw.S``           An incomplete assembly routine for switching process contexts.
``system/ready.c``           A function for adding a process to the ready queue.
``system/resched.c``         The primary scheduling code, equivalent to yield().
``system/getstk.c``          A rudimentary function for dynamically allocating new stacks for new processes.
``compile/Makefile``         Updated rules for compiling XINU.
=======================      ===============================

The ``create()`` and ``ctxsw()`` functions are incomplete and must be
filled in. The file ``system/initialize.c`` contains code to test your
context switch with three processes, each of which prints a process ID
and then yields. Once your context switch and creation functions are
working, you will see these three processes take turns running.

Some Assembly Required
----------------------

An operating system's context switch function typically must be written
in assembly language because the low level manipulations it performs are
not modeled well in higher-level languages. If you have not worked in
Mips assembly language before, there are many helpful resources
available online. Despite its low-level nature, a context switch does
not require complex instructions. Our context switch can be completed
using only arithmetic opcodes, and the load (``lw``), store (``sw``),
and move (``move``) opcodes.

README
------

There are many possible layouts for your operating system's activation
records. It is critical that your ``create()`` function and your
``ctxsw()`` agree on the design you choose. Turn in a plain text file,
``"README"`` that diagrams and explains the activation records created
by ``create()`` and expected by ``ctxsw()``. Label the purpose of each
field. This design document will be worth 25% of the assignment credit.

Testing
-------

The default test case provided with the tarball is necessary, but not
sufficient. Just because it switches between a handful of identical
processes does not guarantee correctness. In embedded systems, details
matter. (A LOT!) Students in previous terms have found that subtle bugs
in this phase of the term project were responsible for nightmares weeks
and months down the line. Test your code thoroughly:

-  Processes can be passed an arbitrary number of parameters upon
   creation;
-  Processes should terminate cleanly upon exit;
-  Stack pointers and activation records should align properly and have
   known contents at critical check points.

You have a working I/O driver, a ``kprintf()`` function for formatted
output, and code from earlier assignments for parsing integers. Use
these to explore every aspect of the operating system structures you are
building.
