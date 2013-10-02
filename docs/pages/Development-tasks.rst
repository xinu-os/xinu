Development Tasks
=================

This page catalogs tasks for the `Embedded Xinu <Embedded Xinu>`__
project. Feel free to add new tasks and change existing tasks as you see
fit.

Pending
-------

Active
~~~~~~

-  USB HID (e.g. keyboard) support
-  SD card support
-  File System/\ `Flash disk driver <Flash driver>`__/TTY disk driver
-  EJTAG interface for debugging, on platforms supporting it
-  Improvements to testsuite (Internal/External Testing and Completion)
-  Memory protection (on some platforms)

Future
------

Software
~~~~~~~~

-  Wireless networking
-  Robo-switch
-  Watchdog timer

Hardware
~~~~~~~~

-  External sensors
-  Poke around WRT54G and WRT350N

Meta
~~~~

-  Windows port of xinu-console tools
-  Rebooter daemon

Completed
---------

Fall 2005
~~~~~~~~~

-  Complete port of ancient-XINU (Pentium) to modern processor (PPC)

Summer 2006
~~~~~~~~~~~

-  Hardware modifications for WRT54GLs
-  Communicate over serial port (MIPS)
-  Write loader for MIPS port

Fall 2006
~~~~~~~~~

-  Complete port of PPC-XINU to MIPS-XINU
-  Interrupt subsystem

Spring 2007
~~~~~~~~~~~

-  Initial asynchronous UART driver (TTY driver)
-  Initial file system (via the second TTY)

Summer 2007
~~~~~~~~~~~

-  Basic Shell (port from x86)
-  Improvements to the `UART Driver <UART Driver>`__ and `TTY
   Driver <TTY Driver>`__
-  Code Audits

   -  Including the startup process, `UART Driver <UART Driver>`__, `TTY
      Driver <TTY Driver>`__, `libxc <Standard library>`__, Interprocess
      Communication (now mailboxes), memory, interrupts, and other
      important XINU subsystems

-  Hardware modifications for EJTAG port

   -  Successfully wrote Flash memory via EJTAG interface

Fall 2007 - Spring 2013
~~~~~~~~~~~~~~~~~~~~~~~

-  No one bothered to update this page, apparently.

Summer 2013
~~~~~~~~~~~

-  Ported to Raspberry Pi
-  Rewrote C library
-  Added USB support
-  Added TFTP and DHCP clients
-  Reduced code duplication

