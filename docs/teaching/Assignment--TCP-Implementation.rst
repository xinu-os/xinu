Assignment: TCP Implementation
==============================

Overview
--------

This assignment is part of the :doc:`Networking-With-Xinu` track for
professors that are :doc:`teaching with Xinu <index>` and it is
intended to be completed in groups of two or three.

Preparation
-----------

A new tar-ball is provided with a solution to the previous assignment.
If your solution is similar to the one presented, you may choose to
continue on with it; but it is suggested that you untar the new project
files in a fresh working directory::

    tar xvzf

Transmission Control Protocol
-----------------------------

Standard number 7 of the :rfc:`Internet Official Protocol Standards
<5000>` is the :wikipedia:`Transmission Control Protocol`. In this
assignment students will build the Transmission Control Protocol into
Embedded Xinu by defining a stream device that uses Embedded Xinu
device paradigm.

Upon completion of the assignment the students' implementation should:

Required Assignment Parts
~~~~~~~~~~~~~~~~~~~~~~~~~

-  send and receive packets of with type TCP
-  determine the status of a specific datagram device and place desired
   data in its stream buffer if applicable

Optional Assignment Parts
~~~~~~~~~~~~~~~~~~~~~~~~~

-  shell integration: add a **tcp-con** command to the shell (establishes
   an active or passive TCP connection)

If you choose not to implement the optional assignment parts then
placing test case code within the shell's **test** command will allow you
to run one or more tests on your implementation at run-time. Optional
portions to an assignment may be required portions of a later
assignment.

Student Outcomes
----------------

Upon completion of this assignment students should understand the key
role this protocol plays in the overall networking architecture. Each
student should also be able to understand how streams as devices fit
into the operating system and the overall networking architecture.

Potential References
--------------------

- :rfc:`Transmission Control Protocol - RFC 793 <793>`
- :wikipedia:`Transmission Control Protocol - Wikipedia <Transmission Control Protocol>`
