Ethernet Driver
===============

The Ethernet driver for Embedded XINU is currently under development.
This page for now will serve as a space for scratch documentation of the
development process.

Failure Log
-----------

11:23, 30 July 2007 (CDT)
~~~~~~~~~~~~~~~~~~~~~~~~~

-  **Problem**: status read 0x00001080 (TimeOut & DescProtoErr)

::

         91556 bytes XINU code.
               [0x80001000 to 0x800175A3]
         32764 bytes stack space.
               [0x800175A4 to 0x8001F59F]
      16648800 bytes heap space.
               [0x8001F5A0 to 0x80FFFFFF]

::

          descriptor rings allocated...
                    receive ring at 0x80020000
                    transmit ring at 0x80021000
            packet pool allocated at 0x800217C0

