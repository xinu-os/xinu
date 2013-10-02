Assignment: IP, ICMP
====================

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

Internet Protocol
-----------------

Two parts of standard number 5 of the `Internet Official Protocol
Standards <http://www.ietf.org/rfc/rfc5000.txt>`__ are the `Internet
Protocol <wikipedia:Internet Protocol>`__ (IP) and the `Internet Control
Message Protocol <wikipedia:Internet Control Message Protocol>`__
(ICMP). In this assignment students will build both IP and ICMP into
Embedded Xinu.

Upon completion of the assignment the students' implementation should:

Required Assignment Parts
~~~~~~~~~~~~~~~~~~~~~~~~~

-  send and receive packets of with type IP
-  further demultiplex IP packets to find the underlying type
-  send and receive ICMP packets

Optional Assignment Parts
~~~~~~~~~~~~~~~~~~~~~~~~~

-  reply to ICMP echo requests properly
-  shell integration: add a *ping* command to the shell (sending out
   echo requests)

If you choose not to implement the optional assignment parts then
placing test case code within the shell's 'test' command will allow you
to run one or more tests on your implementation at run-time. Optional
portions to an assignment may be required portions of a later
assignment.

Student Outcomes
----------------

Upon completion of this assignment students should understand the key
role this protocol plays in the overall networking architecture. Each
student should also be able to understand the unique ties between the
Internet Protocol and the Internet Control Message Protocol.

Potential References
--------------------

-  `Internet Protocol - RFC <http://www.ietf.org/rfc/rfc791.txt>`__
-  `Internet Protocol - Wiki Page <wikipedia:Internet Protocol>`__
-  `Internet Protocol - RFC <http://www.ietf.org/rfc/rfc792.txt>`__
-  `Internet Control Message Protocol - Wiki
   Page <wikipedia:Internet Control Message Protocol>`__

`Category:Networking With Xinu <Category:Networking With Xinu>`__
