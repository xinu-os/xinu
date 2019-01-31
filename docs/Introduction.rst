Introduction
============

.. note::

    This documentation, generated from files distributed with the
    Embedded Xinu source code, is currently under development as a
    replacement for the Embedded Xinu Wiki (http://xinu.mscs.mu.edu).

**Embedded Xinu** is an ongoing research and implementation project in
the area of Operating Systems and Embedded Systems. Its original goal
was to re-implement and port the
:ref:`Xinu Operating System <Xinu>`
to several embedded MIPS platforms, such as the Linksys WRT54GL
router.  Since then, Embedded Xinu has been ported to other platforms,
such as the
:doc:`QEMU MIPSel virtual environment <mips/Mipsel-qemu>`
and the
:doc:`Raspberry Pi </arm/rpi/Raspberry-Pi>`;
see
:ref:`the list of supported platforms <supported_platforms>`.
Although Embedded Xinu is still being developed and ported to new
platforms, a laboratory environment and curriculum materials are
already in use for courses in Operating Systems, Hardware Systems,
Embedded Systems, Networking, and Compilers at Marquette University
and other colleges/universities.

The Embedded Xinu project was conceived and is supervised by
`Dr.  Dennis Brylow <http://www.mscs.mu.edu/~brylow/>`__
and is being conducted by both graduate and undergraduate students in the
:doc:`Systems Laboratory <development/Systems-Laboratory>`
in the
`Math, Statistics, & Computer Science <http://www.mscs.mu.edu/>`__
department of
`Marquette University <http://www.mu.edu/>`__
in Milwaukee, Wisconsin. The first major phase of work on Embedded
Xinu began in the Summer of 2006.

Our project partners include
`Dr. Bina Ramamurthy <http://www.cse.buffalo.edu/~bina/>`__
at University of Buffalo (with whom we shared an
`NSF CCLI <http://www.nsf.gov/pubs/2009/nsf09529/nsf09529.html>`__
grant),
`Dr.  Paul Ruth <http://cs.olemiss.edu/~ruth/wiki/doku.php>`__
at University of Mississippi, and
`Dr. Doug Comer <http://www.cs.purdue.edu/people/comer>`__
(father of Xinu) at Purdue University.

Getting started
---------------

To get started by downloading, compiling, and running Embedded Xinu,
see :doc:`Getting-Started`.

For information about the features of Embedded Xinu, see
:doc:`features/index`.

Information about specific platforms is also available:

- :doc:`/arm/index`
- :doc:`/mips/index`

Teaching with Embedded Xinu
---------------------------

For curriculum guidance on adopting or adapting Embedded Xinu for
undergraduate coursework, see :doc:`teaching/index`.

For information about building an Embedded Xinu laboratory
environment, see :doc:`teaching/Laboratory`.

.. _supported_platforms:

Supported platforms
-------------------

These are all the platforms on which Embedded Xinu currently runs.

.. list-table::
    :widths: 10 10 17 8 8
    :header-rows: 1

    * - Platform
      - Status
      - Comments
      - :ref:`PLATFORM value <makefile_variables>`
      - :ref:`Cross-target <cross_compiler>`
    * - :doc:`Linksys WRT54GL <mips/WRT54GL>`
      - Supported
      - This is our primary development platform, on which Xinu has
        been tested thoroughly.
      - ``wrt54gl``
      - ``mipsel``
    * - Linksys WRT54G v8
      - Supported
      - Tested and running at the Embedded Xinu Lab.  Supported via
        same code as WRT54GL.
      - ``wrt54gl``
      - ``mipsel``
    * - Linksys WRT54G v4
      - Probably Supported
      - The v4 is apparently the version on which WRT54GL is based,
        and so although the Embedded Xinu Lab has not explicitly
        tested it, it probably works.
      - ``wrt54gl``
      - ``mipsel``
    * - Linksys WRT160NL
      - Supported
      - Newer model of WRT54GL. Full O/S teaching core functioning,
        including wired network interface.
      - ``wrt160nl``
      - ``mips``
    * - Linksys E2100L
      - Supported
      - Full O/S teaching core functioning, including wired network
        interface.
      - ``e2100l``
      - ``mips``
    * - ASUS WL-330gE
      - Not actively maintained
      - This platform was working in the past but is no longer being
        actively maintained or tested.
      - ``wl330ge``
      - ``mipsel``
    * - :doc:`mipsel-qemu </mips/Mipsel-qemu>`
      - Supported
      - Full O/S teaching core functioning, network support in progress.
      - ``mipsel-qemu``
      - ``mipsel``
    * - :doc:`Raspberry Pi </arm/rpi/Raspberry-Pi>`
      - Supported
      - Core operating system including wired networking is functional.
      - ``arm-rpi``
      - ``arm-none-eabi``
    * - :doc:`arm-qemu </arm/ARM-qemu>`
      - Supported
      - Core operating system, excluding wired networking, is functional.
      - ``arm-qemu``
      - ``arm-none-eabi``
    * - :doc:`riscv64-qemu </riscv/RISCV-qemu>`
      - Supported
      - Core operating system is functional.
      - ``riscv64-qemu``
      - ``riscv64-unknown-elf``		

.. _Xinu:

The original Xinu
-----------------

The original **Xinu** (**"Xinu is not unix"**) is a small, academic
operating system to teach the concepts of operating systems to
students.  Developed at Purdue University by Dr. Douglas E. Comer in
the early 1980s for the LSI-11 platform, it has now been ported to a
variety of platforms.

**Embedded Xinu** is an update of this project which attempts to
modernize the code base and port the system to modern RISC
architectures such as MIPS, while keeping the original goals of
teaching operating system concepts to students.

.. note::
    Most places in this documentation that simply say "Xinu" or "XINU"
    are actually talking about Embedded Xinu.
