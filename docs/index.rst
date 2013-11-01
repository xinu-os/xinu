Main Page
=========

.. note::

    This documentation, generated from files distributed with the
    Embedded Xinu source code, is currently under development as a
    replacement for the Embedded Xinu Wiki (http://xinu.mscs.mu.edu).

**Embedded Xinu** is an ongoing research and implementation project in
the area of Operating Systems and Embedded Systems. Its original goal
was to re-implement and port the
:doc:`Xinu Operating System <Xinu>`
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

Teaching With Embedded Xinu
---------------------------

-  For curriculum guidance on adopting or adapting Embedded Xinu for
   undergraduate coursework, see :doc:`teaching/index`.
-  Workshops have been held regarding teaching with Embedded Xinu. For
   example, the
   `Teaching With Embedded Xinu Workshop <http://www.cs.olemiss.edu/acmse2010/pdf/xinu.pdf>`__
   at
   `ACMSE 2010 <http://www.cs.olemiss.edu/acmse2010/Home.htm>`__
   in Oxford, Mississippi (Ole Miss campus) shared ready-made
   curriculum resources that have been used successfully to teach
   hardware systems, operating systems, realtime/embedded systems,
   networking, and compilers with the Embedded Xinu platform at
   several colleges/universities.

Building an Embedded Xinu Laboratory
------------------------------------

In this section we are developing instructions so that other groups can
benefit from the work we are doing. These guides can be followed more or
less in order to create a relatively inexpensive platform for a custom
operating system. As our work develops further, there will be more
Xinu-specific information.

#. Obtain a  supported platform (see `Supported Platforms`_).
#. :doc:`Modify the Linksys hardware <mips/HOWTO-Modify-the-Linksys-hardware>`
   or :doc:`Modify the ASUS hardware <mips/HOWTO-Modify-the-ASUS-hardware>`
#. :doc:`Connect to a modified router <mips/HOWTO-Connect-to-a-modified-router>`
#. :doc:`Build Xinu <mips/HOWTO-Build-Xinu>`
#. :doc:`Deploy Xinu <mips/HOWTO-Deploy-Xinu>`
#. (Optional) :doc:`Build a pool of backends <mips/HOWTO-Build-Backend-Pool>`
#. (Recommended) :doc:`Backup your router's factory configuration <mips/HOWTO-Backup-your-router>`

Supported Platforms
-------------------

+---------------+------------+--------------------------------------------------+
| Platform      | Status     | Comments                                         |
+===============+============+==================================================+
| |WRT54GL|     | Supported  | This is our primary development platform, on     |
|               |            | which Xinu has been tested thoroughly.           |
+---------------+------------+--------------------------------------------------+
| |WRT54Gv8|    | Supported  | Tested and running at the Embedded Xinu Lab.     |
|               |            |                                                  |
+---------------+------------+--------------------------------------------------+
| |WRT54Gv4|    | Probably   | The v4 is apparently the version on which        |
|               | Supported  | WRT54GL is based, and so although the Embedded   |
|               |            | Xinu Lab has not explicitly tested it, it        |
|               |            | probably works.                                  |
+---------------+------------+--------------------------------------------------+
| |WRT350N|     | Under      | Currently the synchronous  UART                  |
|               | Development| Driver works.                                    |
+---------------+------------+--------------------------------------------------+
| |WRT160NL|    | Supported  | Newer model of router. Full O/S teaching core    |
|               |            | functioning, including wired network interface.  |
+---------------+------------+--------------------------------------------------+
| |mipselqemu|  | Supported  | Full O/S teaching core functioning, network      |
|               |            | support in progress.                             |
+---------------+------------+--------------------------------------------------+
| |RPI|         | Under      | Core operating system including wired networking |
|               | Development| is functional. Some new features are still being |
|               |            | worked on, and the full documentation (e.g. for  |
|               |            | a laboratory setup) hasn't been completed yet.   |
+---------------+------------+--------------------------------------------------+

.. |WRT54GL|     replace:: :doc:`Linksys WRT54GL   <mips/WRT54GL>`
.. |WRT54Gv8|    replace:: :doc:`Linksys WRT54G v8 <mips/WRT54G>`
.. |WRT54Gv4|    replace:: :doc:`Linksys WRT54G v4 <mips/WRT54G>`
.. |WRT350N|     replace:: :doc:`Linksys WRT350N   <mips/WRT350N>`
.. |WRT160NL|    replace:: :doc:`Linksys WRT160NL  <mips/WRT160NL>`
.. |mipselqemu|  replace:: :doc:`mipsel-qemu       <mips/Mipsel-qemu>`
.. |RPI|         replace:: :doc:`Raspberry Pi      </arm/rpi/Raspberry-Pi>`

Documentation Table of Contents
-------------------------------

.. toctree::
   :maxdepth: 2

   *
   features/index
   mips/index
   arm/index
   teaching/index
   development/index
