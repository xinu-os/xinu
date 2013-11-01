Assignment: IP, ICMP, ARP Applications
======================================

Overview
--------

This assignment is part of the :doc:`Networking-With-Xinu` track for
professors that are :doc:`teaching with Xinu <index>` and it is
intended to be completed in groups of two or three.

Preparation
~~~~~~~~~~~

A new tar-ball is provided with a solution to the previous assignment.
If your solution is similar to the one presented, you may choose to
continue on with it; but it is suggested that you untar the new project
files in a fresh working directory::

  tar xvzf

Shell Commands
--------------

We have implemented three protocols within our networking portion of
Embedded Xinu. In this assignment we will add shell commands to create
useful interaction with those protocols. We will be adding four shell
commands: **arp**, **ping**, **snoop** and **ethstat**.

Upon completion of the assignment the students' implementation should
have all the required shell commands properly implemented and well
tested.

Required Assignment Parts
-------------------------

Shell Command Descriptions
~~~~~~~~~~~~~~~~~~~~~~~~~~

-  **arp** prints out the arp table or performs the modification specified
   via an option.
-  **ping** uses the ICMP protocol's mandatory ECHO\_REQUEST datagram,
   attempting to elicit an ECHO\_RESPONSE from a host or gateway.
-  **snoop** displays packet information for incoming and outgoing network
   packets.

Shell Command Options
~~~~~~~~~~~~~~~~~~~~~

-  **arp**

   -  options for: adding entry, removing entry, sending request and
      clearing the table
   -  add help option that prints usage

-  **ping**

   -  options for: count of pings to be sent and time to live of packets
   -  add help option that prints usage

-  **snoop**

   -  option for: dumping a packet in hex
   -  add help option that prints usage

Optional Assignment Parts
-------------------------

-  respond to received traceroute packets with the proper ICMP packet

If you choose not to implement the optional assignment parts then
placing test case code within the shell's **test** command will allow you
to run one or more tests on your implementation at run-time. Optional
portions to an assignment may be required portions of a later
assignment.

Student Outcomes
----------------

Upon completion of this assignment students should understand how to
implement various shell commands that interact with the underlying
network interface. Implementing the shell commands should give the
student a better grasp of how the various protocols actually work and
how user commands, that they have used elsewhere, disguise this reality.

Potential References
--------------------

-  :RFC:`Ethernet Address Resolution Protocol - RFC <826>`
-  :RFC:`Internet Protocol - RFC <791>`
-  :RFC:`Internet Control Message Protocol - RFC <792>`
-  :wikipedia:`Address Resolution Protocol - Wikipedia <Address Resolution Protocol>`
-  :wikipedia:`Internet Protocol - Wikipedia <Internet Protocol>`
-  :wikipedia:`Internet Control Message Protocol - Wikipedia <Internet Control Message Protocol>`
