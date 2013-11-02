Assignment: Synchronous Serial Driver
=====================================

Overview
--------

This assignment is intended to develop the student's proficiency for
programming in C. This assignment is part of the
:doc:`/teaching/Student-Built-Xinu` track for professors that are :doc:`teaching
with Xinu </teaching/index>`. This assignment should be completed in teams of
two.

Preparation
-----------

You will have to familiarize yourself with several common UNIX tools
for this assignment. The first of these is **tar**, a utility
originally devised to create tape archives for the purpose of backing
files up onto computer tapes.

While **tar** is still used to create tape backups of file systems, it
has become far more common to use **tar** to group files and/or
directories together into a single entity, typically called a
"tar-ball." (So common is the use of **tar** that it has been verbed
in computer science terminology: We speak of "tarring" files, or files
that have been "tarred up.") Tar syntax is somewhat arcane, as **tar**
came into existence before modern standards for command-line options.

Change to your working directory and execute the following command. This
untars the files into your working directory, if the tar-ball was
created properly all the files should go into a subdirectory::

    tar xvzf

For more information on **tar**, please see the
:wikipedia:`UNIX man pages <Manual page (Unix)>`.

Building
--------

While the gcc command-line options provide a great deal of flexibility
when compiling programs, things quickly become unmanageable when the
number of source files exceeds what you can conveniently type in a few
seconds.

The **make** utility can be thought of as a companion to the compiler
infrastructure (preprocessor, compiler, assembler, and linker) that
allows the build rules for large projects to be explicitly encoded in
Makefiles. A Makefile typically consists of common definitions, (such
as, which compiler to use), and a set of rules. Each rule has a target,
such as the file that is to be built, and can be followed by a list of
dependencies and a sequence of steps to perform in order to build that
target. In addition, **make** has quite a few common rules built into it.

You will not have to write your own Makefiles for this course, but you
will have to use and possibly modify some for all of our remaining
assignments. The Makefile is always human-readable, so feel free to open
them up and look around.

To build the Xinu operating system, perform the following steps:

-  Change directory into the top level produced by the tar-ball.
-  Change directory into the subdirectory "compile". This directory
   contains the XINU project Makefile, and is where all of the compiled
   ".o" files will go.
-  Execute the the following command::

    make clean

-  By standard convention, almost all Makefiles include a target called
   "clean" that removes everything except the source code. The tar-ball
   you unpacked already should be clean, but it never hurts to make sure
   that you are starting from a clean slate. You may find yourself using
   this command often.
-  Execute the the following command::

    make

This should produce about a page of output as each source file is
compiled, and the resulting object files are linked together to form the
operating system, a simple set of library functions, and the boot
loader. If all goes as it should, you should find the directory full of
.o files from all of the source code in the other subdirectories, and
most importantly, a newly compiled operating system image called
"xinu.boot."

For more information on **make**, please see the
:wikipedia:`UNIX man pages <Manual page (Unix)>`.

Running
-------

Your Xinu image is now ready to be run on a backend machine. To transfer
it there, we have a special utility called mips-console. Execute
mips-console in the compile directory where your xinu.boot file resides.
Mips-console will connect your terminal to the first available backend
machine, and you should see a message like::

  connection 'xinurouter', class 'mips', host 'xinuserver-hostname'

This will be immediately followed by a stream of automated commands as
the embedded target system boots, configures its network settings, and
uploads your xinu.boot kernel.

The most important thing to remember about mips-console is that it is
modal, like vi. You start out in direct connection mode, in which your
terminal connects directly through special hardware to the serial
console on your backend machine. To get out of mips-console, hit
Control-Space, followed by the 'q' key.

Embedded Xinu Source
--------------------

The source tar-ball we are starting with contains only a few files for
the operating system proper, in the subdirectory system. We will be
adding files into this directory in every subsequent assignment. The
other files in the XINU subdirectories break down as follows:

-  ``compile/`` contains the compilation files for XINU.
-  ``include/`` contains all of our local .h header files used throughout
   the rest of the source code files. At compile time, these are treated
   as being in a standard location, so they can be included with
   "#include<...>" rather than "#include"..."". The header file
   particularly important for this assignment is ``include/uart.h``, which
   contains the structure and constants for the serial port hardware.
-  ``lib/`` contains a small library of standard C functions we can rely
   upon in the operating system, like ``strcmp()`` and ``atoi()``, etc. Remember
   -- the UNIX system libraries are not available to our operating
   system running on the backend!
-  ``loader/`` contains the files for the Mips Xinu boot loader, which
   clears the processor data caches, identifies the processor type, and
   then passes control to the operating system startup code in the
   system subdirectory.

Synchronous Serial Driver
-------------------------

Your task for this assignment is to write a simple synchronous serial
driver for the embedded operating system, so that you can see what you
are doing in all subsequent assignments.

The driver is "synchronous" because it waits for the slow I/O device to
do its work, rather than using interrupts to communicate with the
hardware. We will write the interrupt-driven, "asynchronous" version of
the driver later in the term.

The driver is "serial" because it sends characters one at a time down an
RS-232 serial port interface, like the one found on most modern PC's.

The driver is a "driver" because it provides the software interface
necessary for the operating system to talk to a hardware I/O device.

This platform's serial port, or UART (Universal Asynchronous Receiver /
Transmitter) is a member of the venerable 16550 family of UARTs,
documented in the `UART Specification <http://www.ti.com/litv/snls378b>`__. Of
particular interest to us is section 8 of the specification, starting on
page 14, which describes the registers accessible to programmers. The
UART control and status registers are memory-mapped, starting with base
address 0xB8000300.

The file ``system/kprintf.c`` has the skeleton code for four I/O-related
functions: kputc(), (puts a single character to the serial port,)
kgetc(), (gets a single character from the serial port) kungetc(), (puts
"back" a single character) and kcheckc() (checks whether a character is
available). Each function contains a "TODO" comment where you should add
code. The actual kprintf() is already complete, and will begin working
as soon as you complete the kputc() function it relies upon.
