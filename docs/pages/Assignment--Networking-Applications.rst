Assignment: Networking Applications
===================================

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

Shell Commands
--------------

We have a few more protocols within our networking portion of Embedded
Xinu. In this assignment we will add shell commands to create useful
interaction with those protocols. We will be adding four shell commands:
*tcp-con* and *ftp-receive*.

Upon completion of the assignment the students' implementation should
have all the required shell commands properly implemented and well
tested.

Required Assignment Parts
~~~~~~~~~~~~~~~~~~~~~~~~~

Shell Command Descriptions
~~~~~~~~~~~~~~~~~~~~~~~~~~

-  *tcp-con* establishes a tcp connection with the provided IP address
-  *ftp-receive* uses a TCP/IP connection to transfer acquire a file
   from a provided remote location

Shell Command Options
~~~~~~~~~~~~~~~~~~~~~

-  *tcp-con*

   -  options for: determine whether the connection establishment is
      active or passive
   -  add help option that prints usage

-  *ftp-receive*

   -  options for: determine whether to use passive or active connection
      establishment for file transfer
   -  add help option that prints usage

Student Outcomes
----------------

Upon completion of this assignment students should understand how to
implement various shell commands that interact with the underlying
network interface. Implementing the shell commands should give the
student a better grasp of how the various protocols actually work.

Potential References
--------------------

-  `Ethernet Address Resolution Protocol -
   RFC <http://www.ietf.org/rfc/rfc826.txt>`__
-  `Internet Protocol - RFC <http://www.ietf.org/rfc/rfc791.txt>`__
-  `Internet Control Message Protocol -
   RFC <http://www.ietf.org/rfc/rfc792.txt>`__
-  `Address Resolution Protocol - Wiki
   Page <wikipedia:Address Resolution Protocol>`__
-  `Internet Protocol - Wiki Page <wikipedia:Internet Protocol>`__
-  `Internet Control Message Protocol - Wiki
   Page <wikipedia:Internet Control Message Protocol>`__

`Category:Networking With Xinu <Category:Networking With Xinu>`__
