Assignment: Ultra-Tiny File System
==================================

This assignment is a Xinu assignment allowing the student to more
firmly understand how an operating system works. This assignment is
part of the :doc:`/teaching/Student-Built-Xinu` track for professors that are
:doc:`teaching with Xinu </teaching/index>`. The entire working directory
containing your Xinu operating system will be submission for this
assignment.

Preparation
-----------

First, make a fresh copy of your work thus far::

    cp -R

Untar the new project files on top of this new directory::

    tar xvzf

You should now see the new project files in with your old files. Be
certain to ``make clean`` before compiling for the first time.

Non-blocking ttyRead()
----------------------

To synchronize complex communications properly, it often helps to be
able to "peek" at an I/O device without having to block. Add
non-blocking read as an option to your TTY driver. Use the ``iflags``
field (which should be fetched or set with the ``ttyControl()``
function). Your ``ttyRead()`` function should check the flags and the
count of the input semaphore before each call to ``wait()``. If it looks
like the call might block, return the current count of characters
instead.

The Disk Driver
---------------

The project tarball equips Xinu with a block I/O device driver (running
over the second serial port) that speaks to a ``xinu-disk`` program
running on your computer. The ``xinu-disk`` process maps reading and
writing requests from the backend to a locally-stored disk file. In this
way, your O/S can behave as though it has a small (64K) disk attached,
and the storage is really a file in your home directory.

See below for additional details on the ``xinu-disk`` command.

The File System
---------------

The project tarball includes a partial implementation of a small file
system. The first block of the "disk" is expected to contain a
**superblock** which contains vital bookkeeping information about the
file system, such as:

-  A pointer to a list of free disk blocks,
-  A pointer to a master directory index,
-  Block size, disk size, etc., and
-  A magic number, so that it can tell the difference between a blank
   disk and an initialized disk image.

The new header files ``disk.h`` and ``file.h`` contain the necessary
constants and structures to build the file system. Take a look at
``disk/disk*.c`` (disk driver source code,) ``file/file*.c`` (file
source code,) and ``file/sb*.c`` (superblock source code.)

Our disk has 256 blocks of 256 bytes each. The free disk block list is
dynamic, but our directory index is limited to a fixed number of files,
with fixed length names, and fixed maximum sizes. (The problems and
solutions associated with these limitations would each make excellent
final exam questions, by the way.)

You are to add file deletion (``fileDelete()``) and free block removal
(``sbFreeBlock()``) into the system.

Testing
-------

Provide a main program that demonstrates that your ``fileDelete()`` and
``sbFreeBlock()`` calls are working properly.

The command **xinu-disk** starts up a disk daemon on the local
server that can provide disk access to the selected backend machine via
its second serial port. Build the disk daemon by running
``make xinu-disk`` in your ``compile/`` directory. For example, to
test my file system on backend "Dask" using a local disk image called
"foo.dat", I would run the command ``mips-console dask`` in one
terminal, and then execute ``./xinu-disk foo.dat dask2`` in a second
terminal within the first ten seconds of my Xinu kernel booting.

Xinu-disk's ability to synchronize with a "moving-target" backend is
somewhat rudimentary -- you may occasionally see the handshake fail upon
startup. Just close out the ``mips-console`` session normally, close the
``xinu-disk`` process with a "Control-C", and try again.

The command ``xinu-status`` will list the users on each backend, and
``xinu-status -c uart`` will list the users on each backend's second
serial port. Also recall that a user can bump another user off of a
specific backend after 10 minutes of activity.
