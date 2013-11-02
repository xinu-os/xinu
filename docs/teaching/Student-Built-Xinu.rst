Student Built Xinu
==================

Overview
--------

Having students build their own Xinu is one of the potential tracks
presented for a professor that is :doc:`teaching with Xinu <index>`.

A student built operating system puts the student in the trenches of
operating system development. The student will become intimately
involved with the inner workings of an operating system. This will give
the student a better understanding of the various systems that work
together behind the scenes while an operating system is running.
Operating systems topics that can be incorporated in a student built
Xinu course include: memory management, scheduling, concurrent
processing, device management, file systems and others.

Course Outcomes
---------------

Course development can parallel learning objectives and topics
associated with many Operating Systems courses. [#acm2001]_

Topics
~~~~~~

-  Overview of operating systems
-  Operating system principles
-  Concurrency
-  Scheduling and dispatch
-  Memory management
-  Device management
-  Security and protection
-  File systems
-  Evaluating system performance

Learning Objectives
~~~~~~~~~~~~~~~~~~~

-  Discuss the history of operating systems.
-  Overview of the general and specific purpose of an operating system.
-  Understanding concurrency and state flow diagrams.
-  Understanding deadlock and starvation.
-  Ability to decipher between scheduling algorithms.
-  Understanding the use of memory and virtual memory.
-  Characteristics of serial and parallel devices.
-  Deciphering the concepts behind various file systems
-  Understanding the necessity of security and locating potential system
   security holes

Potential Course Structure
--------------------------

An Operating Systems course using the below course outline or something
similar will introduce students to some fundamental concepts of
operating systems combined with the basics of networking and
communications. Topics include: memory management, scheduling,
concurrent processing, device management, file systems, networking,
security, and system performance. A similar course structure is followed
by `Dr. Dennis Brylow <http://www.mscs.mu.edu/~brylow>`__ at Marquette
University in his sophomore level Operating Systems course. Most of the
assignments where students are building Embedded Xinu are done in teams
of two.

Course Outline
~~~~~~~~~~~~~~

.. list-table::
   :widths: 5 15 20 20 20
   :header-rows: 1

   * - Week
     - Topics
     - Assignments Track One
     - Assignments Track Two
     - Assignments Track Three
   * - 01
     - C (basics) and OS Structures, Processes
     - :doc:`C Basics    <assignments/C-Basics>`
     - :doc:`C Basics    <assignments/C-Basics>`
     - :doc:`C Basics    <assignments/C-Basics>`
   * - 02
     - C (functions, control flow) and Processes
     -
     -
     -
   * - 03
     - C (pointers, arrays, structs) and Threads
     - :doc:`C Structs and Pointers <assignments/C-Structs-and-Pointers>`
     - :doc:`C Structs and Pointers <assignments/C-Structs-and-Pointers>`
     - :doc:`C Structs and Pointers <assignments/C-Structs-and-Pointers>`
   * - 04
     - CPU Scheduling
     - :doc:`Synchronous Serial Driver <assignments/Synchronous-Serial-Driver>`
     - :doc:`Synchronous Serial Driver <assignments/Synchronous-Serial-Driver>`
     - :doc:`SCC Serial Communication`
   * - 05
     - CPU Scheduling
     -
     -
     -
   * - 06
     - Process Synchronization
     - :doc:`Context Switch and Non-Preemptive Scheduling
       <assignments/Context-Switch-and-Non-Preemptive-Scheduling>`
     - :doc:`Context Switch and Non-Preemptive Scheduling
       <assignments/Context-Switch-and-Non-Preemptive-Scheduling>`
     - :doc:`Context Switch and Non-Preemptive Scheduling
       <assignments/Context-Switch-and-Non-Preemptive-Scheduling>`
   * - 07
     - Deadlocks
     - :doc:`Priority Scheduling and Process Termination
       <assignments/Priority-Scheduling-and-Process-Termination>`
     - :doc:`Priority Scheduling & Preemption
       <assignments/Priority-Scheduling-and-Preemption>`
     - :doc:`Priority Scheduling & Preemption
       <assignments/Priority-Scheduling-and-Preemption>`
   * - 08
     - Main Memory and Virtual Memory
     -
     -
     -
   * - 09
     - File System Interface
     - :doc:`Preemption & Synchronization
       <assignments/Preemption-and-Synchronization>`
     - :doc:`Interprocess Communication
       <assignments/Synchronization-and-Interprocess-Communication>`
       or :doc:`LL/SC <assignments/LL-SC>`
     - :doc:`Interprocess Communication
       <assignments/Synchronization-and-Interprocess-Communication>`
   * - 10
     - File System Implementation
     -
     -
     -
   * - 11
     - Mass-Storage Structure
     - :doc:`Delta Queues <assignments/Delta-Queues>`
     - :doc:`Delta Queues <assignments/Delta-Queues>`
     - :doc:`Delta Queues <assignments/Delta-Queues>`
   * - 12
     - I/O Systems
     - :doc:`Heap Memory <assignments/Heap-Memory>`
     - :doc:`Heap Memory <assignments/Heap-Memory>`
     - :doc:`Heap Memory <assignments/Heap-Memory>`
   * - 13
     - Protection, Security and Distributed System Structures
     - :doc:`Asynchronous Device Driver <assignments/Asynchronous-Device-Driver>`
     - :doc:`Ultra-Tiny File System <assignments/Ultra-Tiny-File-System>`
     - Parallel Execution Speedup
   * - 14
     - Distributed System Structures
     -
     -
     -
   * - 15
     - Distributed File Systems
     - :doc:`Ultra-Tiny File System <assignments/Ultra-Tiny-File-System>`
     - :doc:`Basic Networking - Ping <assignments/Basic-Networking---Ping>`
     - Inter-core Message Passing

Books
~~~~~

-  `Abraham Silberschatz, Peter Baer Galvin, and Greg Gagne, Operating
   Systems Concepts, 7th Edition, John Wiley & Sons, ISBN
   #0-471-69466-5. <http://www.os-book.com/>`__

-  `Brian W. Kernighan and Dennis M. Richie, The C Programming Language,
   Prentice-Hall, 1978. <http://netlib.bell-labs.com/cm/cs/cbook/>`__

References
----------

.. [#acm2001] Course topics and learning objectives have been adapted from the
 ACM's `Computing Curricula 2001 Computer Science
 <http://www.acm.org/education/education/education/curric_vols/cc2001.pdf>`__.

Acknowledgements
----------------
This work funded in part by NSF grant DUE-CCLI-0737476.
