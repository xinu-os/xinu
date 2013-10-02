mipsel-qemu
===========

`Embedded Xinu <Embedded Xinu>`__ has been ported to the MIPSel virtual
environment provided by `QEMU <http://wiki.qemu.org/Main_Page>`__. This
provides an easy way to run a basic Embedded Xinu environment on a RISC
architecture without devoting "real" hardware.

Building
--------

`Compile Embedded Xinu <Build Xinu>`__ with ``PLATFORM=mipsel-qemu``.
This will produce the file ``xinu.boot`` in the ``compile/`` directory.

Running
-------

``qemu-system-mipsel -M mips -m 16M -kernel xinu.boot -nographic``

Notes
-----

The mipsel-qemu platform does not yet support networking.
