Assignment: Networking Applications
===================================

Overview
--------

This assignment is part of the :doc:`Networking With Xinu
<Networking-With-Xinu>` track for professors that are :doc:`teaching
with Xinu <index>` and it is intended to be completed in groups of two
or three.

Preparation
-----------

A new tar-ball is provided with a solution to the previous assignment.
If your solution is similar to the one presented, you may choose to
continue on with it; but it is suggested that you untar the new project
files in a fresh working directory::

 tar xvzf

Shell Commands
--------------

We have a few more protocols within our networking portion of Embedded
Xinu. In this assignment we will add shell commands to create useful
interaction with those protocols. We will be adding two shell commands:
**tcp-con** and **ftp-receive**.

Upon completion of the assignment the students' implementation should
have all the required shell commands properly implemented and well
tested.

Required Assignment Parts
~~~~~~~~~~~~~~~~~~~~~~~~~

Shell Command Descriptions
~~~~~~~~~~~~~~~~~~~~~~~~~~

-  **tcp-con** establishes a tcp connection with the provided IP address
-  **ftp-receive** uses a TCP/IP connection to transfer acquire a file
   from a provided remote location

Shell Command Options
~~~~~~~~~~~~~~~~~~~~~

-  **tcp-con**

   -  options for: determine whether the connection establishment is
      active or passive
   -  add help option that prints usage

-  **ftp-receive**

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

-  :RFC:`Ethernet Address Resolution Protocol - RFC <826>`
-  :RFC:`Internet Protocol - RFC <791>`
-  :RFC:`Internet Control Message Protocol - RFC <792>`
-  :wikipedia:`Address Resolution Protocol - Wikipedia <Address Resolution Protocol>`
-  :wikipedia:`Internet Protocol - Wikipedia <Internet Protocol>`
-  :wikipedia:`Internet Control Message Protocol - Wikipedia <Internet Control Message Protocol>`
