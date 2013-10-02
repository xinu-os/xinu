Routing
=======

The routing daemon is automatically started with XINU.

Add a Route
-----------

To add a route to the route entry table use route add with the following
parameters: route add Example: route add 192.168.6.0 192.168.1.100
255.255.255.0 ETH0

Delete a Route
--------------

To delete a route from the route entry table, use route del with the
destination as the third parameter. route del Example: route del
192.168.6.0

Debugging
---------

To enable UDP debugging, uncomment the following line in include/route.h
//#define TRACE\_RT TTY1

In order to see the trace results you open another console to the second
serial port using the router name followed by the number 2. mips-console
router2 Alternatively TTY1 can be changed to TTY0 resulting in the trace
printing on the main console.
