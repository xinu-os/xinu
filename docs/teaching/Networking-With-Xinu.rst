Networking with Xinu
====================

Overview
--------

Having students develop networking aspects with their own, or a
provided, Xinu operating system is one of the potential tracks for a
professor that is :doc:`teaching with Xinu <index>`.

A networking course incorporating Embedded Xinu allows students to
build networking functionality into Embedded Xinu over the period of
the course. Courses may vary in starting point. Some may begin with a
core release of Embedded Xinu, having students implement an Ethernet
driver and develop the entire network stack; others may chose to
utilize an Embedded Xinu release with the Ethernet driver provided,
and have students concentrate on implementing specific protocols
within the network stack. Network stack implementation assignments for
students can parallel various networking lectures that traverse the
stack over the course of the semester, terminating in the students
implementing an application that uses the developed network stack.

Course Outcomes
---------------

Course development can parallel learning objectives and topics
associated with many Communication and Networking courses.
[#acm2001]_

Topics
~~~~~~

-  History of networking.
-  Overview of the specializations within net-centric computing.
-  Network standards.
-  ISO 7-layer reference model
-  Circuit switching and packet switching
-  Streams and datagrams
-  Concepts and services for specific network layers.
-  Protocol and application overview/implementation.
-  Overview of network security.

Learning Objectives
~~~~~~~~~~~~~~~~~~~

-  Discuss the evolution of early networks and the Internet.
-  Explain the hierarchical, layered structure of network architecture.
-  Identify and explain the development of important network standards.
-  Discuss the advantages and disadvantages of different types of
   switching.
-  Demonstrate how a packet traverses the Internet.
-  Implement a simple network using devices running the Embedded Xinu
   operating system.
-  Discuss and explain the reasoning for network security.

Potential Course Structure
--------------------------

The students will use a base Embedded Xinu release with an Ethernet
driver; this kernel can be professor provided or student built in
previous courses. All assignments provided below (after the first one)
are intended for groups of two or three students.

Optionally, with each new assignment professors may provide students
with a proper implementation of the previous assignment. This allows
students to concentrate on implementing the current assignment and
avoid distractions caused by implementation blunders in previous
assignments.  Alternatively, students can utilizing their same code
base throughout the semester, learning the importance of correcting
prior mistakes.

Course Outline
~~~~~~~~~~~~~~

.. list-table::
    :widths: 5 20 30
    :header-rows: 1

    * - Week
      - Topics
      - Assignments
    * - 01
      - History of networking and the Internet & specializations of net-centric computing
      - :doc:`Networking Standards <Assignment--Networking-Standards>`
    * - 02
      - Networking standards & 7-layer ISO model
      - :doc:`Packet Demultiplexing <Assignment--Packet-Demultiplexing>`
    * - 03
      - Ethernet & Address Resolution Protocol
      - :doc:`Implementing ARP <Assignment--ARP>`
    * - 04
      - Internet Protocol
      - :doc:`Implementing IP & ICMP <Assignment--IP-ICMP>`
    * - 05
      - Internet Protocol and Internet Control Message Protocol
      -
    * - 06
      - Internet Packet Traversal, Security Concerns for IP & ARP
      - :doc:`IP, ICMP, and ARP Applications <Assignment--IP-ICMP-ARP-Applications>`
    * - 07
      - Datagrams - UDP
      - :doc:`UDP Development and Implementation <Assignment--UDP-Implementation>`
    * - 08
      - Datagrams - UDP, Dynamic Host Configuration Protocol
      -
    * - 09
      - Dynamic Host Configuration Protocol
      - :doc:`DHCP Development and Implementation <Assignment--DHCP-Implementation>`
    * - 10
      - Streams - TCP
      -
    * - 11
      - Security Concerns for UDP and TCP
      - :doc:`TCP Development and Implementation <Assignment--TCP-Implementation>`
    * - 12
      - Interaction Protocols for Networked Devices
      -
    * - 13
      - Wireless Networking
      -
    * - 14
      - Network Based Application Development
      - :doc:`Network Based Applications <Assignment--Networking-Applications>`
    * - 15
      - Networking Future            
      -

Student Outcomes from Completion of Course Assignments
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Upon completion of all assignments the student should have a grasp of
the networking architecture that he or she implemented over the whole
course. The student should be able to answer questions about all
implemented protocols as well as general questions about other
non-implemented protocols. The student should also be able to understand
the complexities of their implementation. Given the full implementation
of the networking architecture the student should be able to pin-point
locations in the architecture where optimization is possible and the
difficulty involved.

In addition, students that completed all the assignments should have a
grasp of devices, user interaction and driver/OS interaction within
Embedded Xinu. Other operating system concepts, including threads,
memory management, interprocess communication and synchronization, are
reinforced through the use of Embedded Xinu.

Books
~~~~~

-  Currently this course structure has no suggested books.

References
----------

.. [#acm2001] Course topics and learning objectives have been adapted from the
 ACM's `Computing Curricula 2001 Computer Science
 <http://www.acm.org/education/education/education/curric_vols/cc2001.pdf>`__.

Acknowledgements
----------------

This work funded in part by NSF grant DUE-CCLI-0737476.
