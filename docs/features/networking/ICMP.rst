ICMP
====

As part of its :doc:`Networking subsystem <index>`, XINU supports
the **Internet Control Message Protocol** (**ICMP**).  The support can
be found in the :source:`network/icmp/` directory.  This module
features an ICMP daemon that runs on start up and responds to ICMP
echo requests(pings).  The module implements the function
:source:`icmpEchoRequest() <network/icmp/icmpEchoRequest.c>`, which is
used by the **ping** :doc:`shell command </features/Shell>` to send
ICMP echo requests to the specified IPv4 address.

Note that order to either send or reply to ICMP echo packets, the
network interface needs to be brought up (e.g. by using the **netup**
at the shell).

Resources
---------

- :rfc:`792`
