UDP
===

Resources
---------

XINU implements UDP as per RFC 768

Debugging
---------

To enable UDP debugging, uncomment the following line in include/udp.h
//#define TRACE\_UDP TTY1

In order to see the trace results you open another console to the second
serial port using the router name followed by the number 2. mips-console
router2 Alternatively TTY1 can be changed to TTY0 resulting in the trace
printing on the main console.
