arm-qemu
========

|EX| has been ported to an ARM virtual
environment provided by `QEMU <http://qemu.org>`__.  Specifically,
the supported emulation environment consists of an ARM1176 CPU
combined with the peripherals of the ARM Versatile Platform Baseboard.
These peripherals consist of two ARM Dual Timer Module (SP804), a
PrimeCell Vectored Interrupt Controller (VIC) (PL011), four PrimeCell
UARTs (PL011), and other devices.  Similar to the
:doc:`/mips/Mipsel-qemu` port, the *arm-qemu* port of Embedded Xinu
provides an easy way to run a basic Embedded Xinu environment on a
RISC architecture without devoting "real" hardware.

Building
--------

:ref:`Compile Embedded Xinu <compiling>` with ``PLATFORM=arm-qemu``.
Note that this requires a :ref:`cross compiler <cross_compiler>`
targeting ARM.  This will produce the file ``xinu.boot`` in the
``compile/`` directory.

Running
-------

::

    $ qemu-system-arm -M versatilepb -cpu arm1176 -m 128M -nographic -kernel xinu.boot

Note the ``-cpu arm1176`` option.  This requires QEMU v1.0 (released
December 2011) or later.

Notes
-----

The ``arm-qemu`` platform does not yet support networking.
