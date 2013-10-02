Assignment: UDP Implementation
==============================

Overview
--------

This assignment is part of the `Networking With
Xinu <Networking With Xinu>`__ track for professors that are `Teaching
With Xinu <Teaching With Xinu>`__ and it is intended to be completed in
groups of two or three.

Preparation
-----------

A new tar-ball is provided with a solution to the previous assignment.
If your solution is similar to the one presented, you may choose to
continue on with it; but it is suggested that you untar the new project
files in a fresh working directory: tar xvzf

User Datagram Protocol
----------------------

Standard number 6 of the `Internet Official Protocol
Standards <http://www.ietf.org/rfc/rfc5000.txt>`__ is the `User Datagram
Protocol <wikipedia:User Datagram Protocol>`__. In this assignment
students will build the User Datagram Protocol into Embedded Xinu by
defining a datagram device that uses Embedded Xinu device paradigm.

Upon completion of the assignment the students' implementation should:

Required Assignment Parts
~~~~~~~~~~~~~~~~~~~~~~~~~

-  send and receive packets of with type UDP
-  determine the status of a specific datagram device and place desired
   datagram in buffer if applicable

Optional Assignment Parts
~~~~~~~~~~~~~~~~~~~~~~~~~

-  shell integration: add a *traceroute* command to the shell (sending
   out proper UDP packets)

If you choose not to implement the optional assignment parts then
placing test case code within the shell's 'test' command will allow you
to run one or more tests on your implementation at run-time. Optional
portions to an assignment may be required portions of a later
assignment.

Student Outcomes
----------------

Upon completion of this assignment students should understand the key
role this protocol plays in the overall networking architecture. Each
student should also be able to understand how datagrams as devices fit
into the operating system and the overall networking architecture.

Potential References
--------------------

-  `User Datagram Protocol - RFC
   768 <http://www.ietf.org/rfc/rfc768.txt>`__
-  `User Datagram Protocol - Wiki
   Page <wikipedia:User Datagram Protocol>`__

`Category:Networking With Xinu <Category:Networking With Xinu>`__
