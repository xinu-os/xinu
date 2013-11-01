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
see `Supported Platforms`_.
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

Teaching with Embedded Xinu
---------------------------

For curriculum guidance on adopting or adapting Embedded Xinu for
undergraduate coursework, see :doc:`teaching/index`.

Building an Embedded Xinu laboratory
------------------------------------

In this section we are developing instructions so that other groups
can benefit from the work we are doing. These guides can be followed
more or less in order to create a relatively inexpensive platform for
a custom operating system. As our work develops further, there will be
more Xinu-specific information.

#. Obtain a  supported platform (see `Supported Platforms`_).
#. :doc:`Modify the Linksys hardware <mips/HOWTO-Modify-the-Linksys-hardware>`
   or :doc:`Modify the ASUS hardware <mips/HOWTO-Modify-the-ASUS-hardware>`
#. :doc:`Connect to a modified router <mips/HOWTO-Connect-to-a-modified-router>`
#. :doc:`Build Xinu <Getting-Started>`
#. :doc:`Deploy Xinu <mips/HOWTO-Deploy-Xinu>`
#. (Optional) :doc:`Build a pool of backends <mips/HOWTO-Build-Backend-Pool>`
#. (Recommended) :doc:`Backup your router's factory configuration <mips/HOWTO-Backup-your-router>`

.. _supported_platforms:

Supported platforms
-------------------

.. list-table::
    :widths: 10 20 30
    :header-rows: 1
    
    * - Platform
      - Status
      - Comments
    * - |WRT54GL|
      - Supported
      - This is our primary development platform, on which Xinu has
        been tested thoroughly.
    * - |WRT54Gv8|
      - Supported
      - Tested and running at the Embedded Xinu Lab.
    * - |WRT54Gv4|
      - Probably Supported 
      - The v4 is apparently the version on which WRT54GL is based,
        and so although the Embedded Xinu Lab has not explicitly
        tested it, it probably works.
    * - |WRT350N|
      - Under Development
      - Currently the synchronous UART Driver works.
    * - |WRT160NL|
      - Supported
      - Newer model of router. Full O/S teaching core functioning,
        including wired network interface.
    * - |mipselqemu|
      - Supported
      - Full O/S teaching core functioning, network support in progress.
    * - |RPI|
      - Under Development
      - Core operating system including wired networking is
        functional. Some new features are still being worked on, and
        the full documentation (e.g. for a laboratory setup) hasn't
        been completed yet.

.. |WRT54GL|     replace:: :doc:`Linksys WRT54GL   <mips/WRT54GL>`
.. |WRT54Gv8|    replace:: :doc:`Linksys WRT54G v8 <mips/WRT54G>`
.. |WRT54Gv4|    replace:: :doc:`Linksys WRT54G v4 <mips/WRT54G>`
.. |WRT350N|     replace:: :doc:`Linksys WRT350N   <mips/WRT350N>`
.. |WRT160NL|    replace:: :doc:`Linksys WRT160NL  <mips/WRT160NL>`
.. |mipselqemu|  replace:: :doc:`mipsel-qemu       <mips/Mipsel-qemu>`
.. |RPI|         replace:: :doc:`Raspberry Pi      </arm/rpi/Raspberry-Pi>`

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
