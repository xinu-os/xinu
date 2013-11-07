mipsel-qemu
===========

|EX| has been ported to the MIPSel (little-endian MIPS) virtual
environment provided by `QEMU <http://qemu.org>`__.  This provides an
easy way to run a basic Embedded Xinu environment on a RISC
architecture without devoting "real" hardware.

Building
--------

:ref:`Compile Embedded Xinu <compiling>` with
``PLATFORM=mipsel-qemu``.  Note that this requires a :ref:`cross
compiler <cross_compiler>` targeting little-endian MIPS ("mipsel").
This will produce the file ``xinu.boot`` in the ``compile/``
directory.

Running
-------

::

    $ qemu-system-mipsel -M mips -m 16M -kernel xinu.boot -nographic

Notes
-----

The ``mipsel-qemu`` platform does not yet support networking.
