Assignment: Asynchronous Device Driver
======================================

This assignment is a Xinu assignment allowing the student to more firmly
understand how an operating system works. This assignment is part of the
:doc:`/teaching/Student-Built-Xinu` track for professors that are
:doc:`teaching with Xinu </teaching/index>`. The entire working directory containing your Xinu
operating system will be submission for this assignment.

Preparation
-----------

First, make a fresh copy of your work thus far::

 cp -R

Untar the new project files on top of this new directory::

 tar xvzf

You should now see the new project files in with your old files. Be certain to
``make clean`` before compiling for the first time.

Semaphore Loose Ends
--------------------

Modify ``system/scount.c``, ``system/sreset.c``, ``system/freesem.c``,
and ``system/signaln.c`` to finish off the interface for our semaphore
subsystem. The ansynchronous driver will require all of these functions
to maintain proper syncronization, but none of these semaphore helper
functions is more than a few lines of code.

Dr. Dennis Brylow, quoting one of his students:

    Question: We are looking at ``sreset`` and ``freesem``. When it
    says" release the process from the semaphore's waiting queue," what
    exactly are we supposed to do with the processes once they are
    released from the semaphore? Should we kill them, or put them back
    into the ready queue?

    Answer: Put them back into the ready queue.

Device Driver
-------------

The project tarball includes the following files, which comprise the
device interface layer for Xinu: ``system/devtable.c``,
``system/close.c``, ``system/control.c``, ``system/getc.c``,
``system/open.c``, ``system/putc.c``, ``system/read.c``,
``system/write.c``, and a revised ``system/initialize.c``. The device
layer allows user programs (like the processes in ``main.c``) to work
with device-specific functions using standard names like ``open()`` and
``read()``, rather than have to explicitly name the underlying device
driver.

You do **NOT** need to make any changes to the device interface layer
files. However, you do need to know what they are doing. Trace through a
few of the high-level device functions. They are very short, and almost
all the same. The file ``system/devtable.c`` contains the master device
table declarations.

The Asynchronous TTY Driver
---------------------------

For this project, you will be implementing an asynchronous device driver
for an interrupt-driven UART. The difference between the existing
synchronous UART driver and the new asynchronous tty driver is that the
synchronous driver always waits for the serial port hardware to complete
a read or write before returning. A proper asynchronous driver does not
wait needlessly for the hardware, but instead only communicates with the
UART when a hardware interrupt indicates to the system that it is ready
for the next batch of work.

The Universal Asynchronous Receiver/Transmitter (UART) on our hardware
seems to be largely compatible with the venerable National Semiconductor
16550 family of UARTs that have been reliably driving serial ports in
PC's for decades. The PDF of the `device
documentation <http://www.national.com/ds.cgi/NS/NS16C552.pdf>`__ will
explain the control and status registers (CSRs) on this device. The base
address of its memory-mapped I/O region is noted in the master device
table, devtab.

The Upper Half of a driver consists of the functions called by various
user-level processes, functions such as ``ttyRead`` and ``ttyWrite``. As
much as possible, the upper half functions shield the user from the
messy details of the hardware.

The Lower Half of a driver consists of the handler functions run when
hardware interrupt requests arrive. Lower half functions must do their
work quickly, and deal with data that has been buffered up by the upper
half functions.

You will be working on the upper half functions ``device/tty/ttyRead.c``
and ``device/tty/ttyWrite.c`` and the lower half function
``device/tty/ttyIntr.c``. The ``ttyIntr()`` function is called via a new
hook in ``system/xtrap.c`` when a hardware interrupt request from the
UART hardware arrives.

Look carefully at the structure of the asynchronous driver suggested by
the structures in ``include/tty.h``. It is critically important that you
be able to envision how the upper and lower halves of the driver should
interact **before** you start adding code to the system.

Testing
-------

Provide a main program that demonstrates that your asynchronous driver
is working properly.

The Marquette University Embedded MIPS backends have two serial port
connections. All of the backends in the lab have their second serial
port available via the xinu-console system. When running
``mips-console`` normally, note the name of the backend you are
allocated. Append a "2" to the name, and run the command
``xinu-console name2`` to gain access to the second serial port for
backend "name." The xinu-console reservation system tracks these two
connections independently. Please be courteous to your neighbors and
release any consoles as soon as you are done.

The "2" is assigned for ease of use, a Xinu lab may set up individual
names for access to the second serial port on a backend and thus the
user should understand what standard the lab he or she works with before
assuming a naming convention is used.

The command "``xinu-status``\ " will list the users on each backend, and
"``xinu-status -c uart``\ " will list the users on each backend's second
serial port. Also recall that a user can bump another user off of a
specific backend after 10 minutes of activity.
