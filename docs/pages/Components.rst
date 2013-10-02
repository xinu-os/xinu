Components
==========

XINU can be easily understood as consisting of conceptual "components,"
or subsystems. Most components have their own initialization function
that sets up global data structures necessary to function properly. The
subsystems currently within XINU include:

-  Loader
-  System

   -  Process management
   -  Semaphores
   -  Memory allocation
   -  Interrupt subsystem

-  Devices

   -  `UART driver <UART driver>`__
   -  `TTY driver <TTY driver>`__
   -  `Flash driver <Flash driver>`__
   -  `Backplane driver <Backplane driver>`__

-  Shell (xsh)
-  Mailbox
-  Testsuite
-  Libraries

   -  `Standard C library <Standard_library>`__
   -  Pthread (minimal)


