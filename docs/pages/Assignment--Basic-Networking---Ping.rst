Assignment: Basic Networking (``ping``)
=======================================

This assignment is a Xinu assignment allowing the student to more firmly
understand how an operating system works. This assignment is part of the
`Student Built Xinu <Student Built Xinu>`__ track for professors that
are `Teaching With Xinu <Teaching With Xinu>`__. The entire working
directory containing your Xinu operating system will be submission for
this assignment.

Preparation
-----------

First, make a fresh copy of your work thus far. cp -R Untar the new
project files on top of this new directory: tar xvzf You should now see
the new project files in with your old files. Be certain to
``make clean`` before compiling for the first time.

The Xinu Shell
--------------

With the addition of a full-featured TTY driver in the previous
assignment, we can now add the command-line Xinu user interface, the
*Xinu Shell* to the system. The new tarball includes a new subdirectory
*shell/* that provides the I/O processing necessary to parse user input
and launch a small set of commands. Several useful commands are provided
as examples. This assignment will be concerned primarily with the *ping*
and *pingserver* commands.

Ethernet Driver
---------------

This project tarball equips your Xinu kernel with a block-oriented,
asynchronous ethernet driver for the router's built-in BCM4713 network
interface. You are not required to modify the ethernet driver (directory
*device/ether/*), but it wouldn't be a bad idea to familiarize yourself
with its workings. It follows the same standard device abstraction as we
have seen in the TTY and disk device drivers.

Debugging Tools
---------------

For your convenience, we have also provided a *snoop()* function in
*network/snoop.c*, which will attempt to provide human-readable
interpretations of packets when called. This is provided strictly for
debugging purposes. It takes a buffer that contains an ethernet packet
and a designated length and prints out various values as well as a
`hexdump <wikipedia:Hexdump>`__ of the packet.

Student Implementation
----------------------

Your task for this assignment is to implement a version *ping* that
allows your main programs to send *echo request* packets to other
machines and allows other machines to send *echo requests* packets to
your allocated backend. This project can be split into two distinct
parts, the client and the server. You should first implement a server
that allows other machines to *ping* your backend while it is running.
The second part of this project is to implement the client side which
will allow you to write a main process that can *ping* other machines.
In each of these parts you will deal with two types of ICMP packets,
*echo request* and *echo reply* packets. The *request* packets are sent
from a client to a target machine that is running a ping server. The
*reply* packets are sent in response to a client's request back to the
client.

Echo Reply (Ping) Server
~~~~~~~~~~~~~~~~~~~~~~~~

The Ping Server is a process spawned by the *pingserver* shell command
to listen to the Ethernet device for ping requests and construct correct
replies. (See `Echo Reply <wikipedia:Ping>`__.) Due to the simplified
nature of packet de-multiplexing in our current system, it will not be
possible for your Xinu kernel to run both a ping server and a ping
client simultaneously. (The two processes will "fight" over incoming
packets.) Instead, use a second router to generate ping requests to test
your server, or use the Linux command "*ping*\ " on a local server which
has direct access to the private Xinu Network.

Echo Request (Ping) Client
~~~~~~~~~~~~~~~~~~~~~~~~~~

The project tarball includes a starter stub for a ping command in the
shell directory file *shell/xsh\_ping.c*. Your ping command should take
a single command-line argument consisting of a destination IP address in
dotted decimal notation. It should

-  generate a sequence of 20 (*MAX\_REQUESTS* in *include/network.h*)
   ping packets at one second intervals;
-  watch for ping replies from the target machine;
-  print out replies received (see the *icmp\_reqreply* function in
   *network/icmp.c*); and
-  print out a summary of results.

The proper term for a "ping packet" is an
`ICMP <wikipedia:Internet_Control_Message_Protocol>`__ `Echo
Request <wikipedia:Ping>`__. An answering packet is an Echo Reply. For
the sake of simplicity, this project will not implement a full
`ARP <wikipedia:Address_Resolution_Protocol>`__ system to resolve IP
addresses down to underlying Ethernet `MAC <wikipedia:MAC_address>`__
Addresses. Instead, we will use a predefined static table,
"*arp\_map*\ " to match requested ping IP addresses for machines local
to the private Xinu Network. Any address that is not known to the
*arp\_map* table will be assumed to be on another network, and the ping
packet should instead be addressed to Zardoz, the Xinu Network Gateway.
The gateway's MAC address is hard-coded in *network.h* with the constant
*PING\_GATEWAY*.

Resources
---------

-  `Wikipedia: Ping <wikipedia:Ping>`__
-  `RFC 792 - Internet Control Message Protocol
   (ICMP) <http://tools.ietf.org/html/rfc792>`__
-  `Wikipedia: Internet Control Message Protocol
   (ICMP) <wikipedia:Internet_Control_Message_Protocol>`__

`Category:Student Built Xinu <Category:Student Built Xinu>`__
