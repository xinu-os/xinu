Routing
=======

As part of its :doc:`Networking subsystem <index>`, XINU supports IPv4
:wikipedia:`routing`.

At the center of the routing module is the *routing daemon*, which
repeatedly retrieves packets from a :doc:`mailbox <Mailboxes>` on
which packets can be placed using :source:`rtRecv()
<network/route/rtRecv.c>` and routes them to their destinations (or
sends special packets such as :doc:`ICMP` unreachable packets) by
calling :source:`rtSend() <network/route/rtSend.c>`.  To route
packets, the routing daemon makes use of a routing table.  Routing
table entries can be programatically added and removed with
:source:`rtAdd() <network/route/rtAdd.c>` and :source:`rtRemove()
<network/route/rtRemove.c>`, respectively.  The shell command
**route** relies on both these functions.

Add a Route
-----------

To add a route to the route entry table use **route add** with the
following parameters:

.. code-block:: none
   
    route add <destination> <gateway> <mask> <interface>

Example:

.. code-block:: none

    route add 192.168.6.0 192.168.1.100 255.255.255.0 ETH0

Relevant source code:
:source:`xsh_route() <shell/xsh_route.c>`,
:source:`rtAdd() <network/route/rtAdd.c>`

Delete a Route
--------------

To delete a route from the route entry table, use **route del** with
the destination as the third parameter:

.. code-block:: none

    route del <destination>
    
Example:

.. code-block:: none

    route del 192.168.6.0

Relevant source code:
:source:`xsh_route() <shell/xsh_route.c>`,
:source:`rtRemove() <network/route/rtRemove.c>`.

Debugging
---------

The routing subsystem contains :doc:`Trace statements
</development/Trace>` for debugging.  To enable, uncomment the
following line in :source:`include/route.h`, and optionally change the
device (such as TTY1) to which messages will be logged::

    // #define TRACE_RT TTY1
