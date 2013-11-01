Assignment: Heap Memory
=======================

This assignment is a Xinu assignment allowing the student to more
firmly understanding of how an operating system works. This assignment
is part of the :doc:`Student-Built-Xinu` track for professors that are
:doc:`teaching with Xinu <index>`. The entire working directory
containing your Xinu operating system will be submission for this
assignment.

Preparation
-----------

First, make a fresh copy of your work thus far. cp -R Untar the new
project files on top of this new directory: tar xvzf You should now see
the new project files in with your old files. Be certain to make clean
before compiling for the first time.

Getmem and Freemem
------------------

The files ``include/memory.h`` and ``system/initialize.c`` define and
initialize a free list of available memory blocks on the heap. Implement
``system/getmem.c`` and ``system/freemem.c`` to dole out blocks of
requested memory and properly maintain the free list. Use First Fit with
simple compaction. (Compaction is the "stretch" problem in this
assignment -- get everything else working first.)

Delete the file ``system/getstk.c`` and replace the lone call to this
function in ``system/create.c`` with a proper call to ``getmem()``
instead. Modify ``system/kill.c`` to call ``freemem()`` to deallocate a
process stack when a process dies.

Grading
-------

Project credit will be divvied up evenly between getting memory, freeing
memory, and compaction. By now, you should be aware that rigorous
testing is the key to success in these projects. Our test cases will
include at least:

-  Verifying that your ``getmem()`` and ``freemem()`` functions operate
   correctly over a variety of request sizes
-  Checking your memory free list under a wide variety of conditions; if
   you are not writing a helper function to print out your free list,
   you are probably not serious about understanding what your system is
   doing
-  Allocating and freeing as much memory as possible, (How can you tell
   whether you have a memory leak?)
-  Verifying that your ``freemem()`` compaction works.
