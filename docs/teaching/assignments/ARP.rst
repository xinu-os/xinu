Assignment: ARP
===============

.. epigraph::

    The world is a jungle in general, and the networking game contributes many
    animals.

    --David C. Plummer [#]_

Overview
--------

This assignment is part of the :doc:`/teaching/Networking-With-Xinu` track for
professors that are :doc:`teaching with Xinu </teaching/index>` and it is
intended to be completed in groups of two or three.

Preparation
-----------

A new tar-ball is provided with a solution to the previous assignment.
If your solution is similar to the one presented, you may choose to
continue on with it; but it is suggested that you untar the new project
files in a fresh working directory: ``tar xvzf``

Address Resolution Protocol
---------------------------

Standard 37 of the :rfc:`Internet Official Protocol Standards <5000>` is the
:wikipedia:`Address Resolution Protocol`. In this assignment students will build
the Address Resolution Protocol into Embedded Xinu.

Upon completion of the assignment the students' implementation should:

Required Assignment Parts
-------------------------

-  add and remove ARP table entries
-  send, receive and process ARP requests
-  have ARP entries time out and be removed
-  lookup MAC address from table (given an IP) and return it to help
   fill in packets before sending them

Optional Assignment Parts
-------------------------

-  shell integration: add an **arp** command to the shell

   -  options for: adding entry, removing entry, sending request and
      clearing the table

If you choose not to implement the optional assignment parts then
placing test case code within the shell's 'test' command will allow you
to run one or more tests on your implementation at run-time. Optional
portions to an assignment may be required portions of a later
assignment.

Student Outcomes
----------------

Upon completion of this assignment students should understand the key
role this protocol plays in the overall networking architecture. Each
student should be able to understand what an Internet Protocol address
is and how the protocol uses this address to acquire a MAC address for
the network packet before it can proceed through the system and be sent
out over the network.

Potential References
--------------------

- :rfc:`826`
- :wikipedia:`Address Resolution Protocol`

Notes
-----
.. [#] :rfc:`826`
