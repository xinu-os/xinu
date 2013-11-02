Assignment: Packet Demultiplexing
=================================

Overview
--------

This assignment is part of the :doc:`Networking With Xinu
</teaching/Networking-With-Xinu>` track for professors that are :doc:`teaching
with Xinu </teaching/index>` and it is intended to be completed in groups of two
or three.

Preparation
~~~~~~~~~~~

You will have to familiarize yourself with several common UNIX tools for
this assignment. The first of these is **tar**, a utility originally devised
to create tape archives for the purpose of backing files up onto
computer tapes.

While **tar** is still used to create tape backups of file systems, it has
become far more common to use **tar** to group files and/or directories
together into a single entity, typically called a "tar-ball." (So common
is the use of **tar** that it has been verbed in computer science
terminology: We speak of "tarring" files, or files that have been
"tarred up.") Tar syntax is somewhat arcane, as **tar** came into existence
before modern standards for command-line options.

Change to your working directory and execute the following command. This
untars the files into your working directory, if the tar-ball was
created properly all the files should go into a subdirectory::

    tar xvzf

For more information on tar, please see the
:wikipedia:`UNIX man pages <Manual page (Unix)>`.

Building
~~~~~~~~

While the **gcc** command-line options provide a great deal of flexibility
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
target. In addition, make has quite a few common rules built into it.

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

For more information on make, please see the
:wikipedia:`UNIX man pages <Manual page (Unix)>`.

Running
~~~~~~~

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

Packet Demultiplexing
---------------------

The packet demultiplexing assignment will have students implement a way
for the Embedded Xinu operating system to interact with the ethernet
driver, both receiving and sending of packets. Incoming packets should
be properly divided up (demultiplexed) and passed along to the next part
of the system or stored. Outgoing packets should have the final ethernet
header fields filled in before sending it off to the driver.

Assignment Notes
~~~~~~~~~~~~~~~~

-  Implementation can vary based on how the professor intends to grade
   the assignment

   -  Have the students insert print outs of what type of packet is
      received and print something out when one is sent out
   -  Give the students some portion of code so they can pass the packet
      onward to a piece of the operating system they have yet to write

Student Outcomes
~~~~~~~~~~~~~~~~

With the completion of the assignment students should understand how
demultiplexing works and have knowledge about the complexities of
optimizing packet demultiplexing. Students should have a feel for how
the Embedded Xinu operating system interacts with the ethernet driver
and understand that initial/final crucial step for network
communication.
