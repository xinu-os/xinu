TCP
===

Examples
--------

`TCP Echo Test <TCP Echo Test>`__

Debugging
---------

To enable TCP debugging, uncomment the following line in include/tcp.h
//#define TRACE\_TCP TTY1

In order to see the trace results you open another console to the second
serial port using the router name followed by the number 2. mips-console
router2 Alternatively TTY1 can be changed to TTY0 resulting in the trace
printing on the main console.

Resources
---------

RFC 793
