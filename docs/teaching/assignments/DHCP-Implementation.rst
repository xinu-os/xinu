Assignment: DHCP Implementation
===============================

Overview
--------

This assignment is part of the :doc:`/teaching/Networking-With-Xinu` track for
professors that are :doc:`teaching with Xinu </teaching/index>` and it is
intended to be completed in groups of two or three.

Preparation
~~~~~~~~~~~

A new tar-ball is provided with a solution to the previous assignment.
If your solution is similar to the one presented, you may choose to
continue on with it; but it is suggested that you untar the new project
files in a fresh working directory::

 tar xvzf

Dynamic Host Configuration Protocol
-----------------------------------

The :wikipedia:`Dynamic Host Configuration Protocol` provides a
framework for passing configuration information to hosts on a TCPIP
network. DHCP is based on the Bootstrap Protocol (BOOTP). In this
assignment students will build a DHCP client into Embedded Xinu that
dynamically acquires networking information for the operating system
to use.

Upon completion of the assignment the students' implementation should:

Required Assignment Parts
~~~~~~~~~~~~~~~~~~~~~~~~~

-  implement a functional DHCP client within Embedded Xinu

Student Outcomes
----------------

Upon completion of this assignment students should understand the key
role this protocol plays in the overall networking architecture.

Potential References
--------------------

- :rfc:`2131`
- :wikipedia:`Wikipedia - Dynamic Host Configuration Protocol <Dynamic Host Configuration Protocol>`
