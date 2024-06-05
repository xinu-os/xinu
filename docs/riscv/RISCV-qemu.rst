riscv64-qemu
============

|EX| has been ported to an emulated RISC-V 64
environment provided by QEMU.
Currently the port supports running on the emulated 'virt'
machine that features

- CLINT - Core-Local Interruptor (support for timer, exceptions)
  
- PLIC - Platform-Level Interrupt Controller (interrupts for external devices)
  
- 16550A UART
  
- VirtIO network and disk devices (currently still not supported by Xinu)
  
- device-tree (used to configure memory size, timer, CLINT and PLIC)
  
- RISC-V Privileged ISA v1.10



Building
--------

:ref:`Compile Embedded Xinu <compiling>` with ``PLATFORM=riscv64-qemu``.
Note that this requires a :ref:`cross compiler <cross_compiler>`
and binary utils targeting the RISCV-64 ISA
(riscv64-unknown-elf-gcc, etc).
The build process has been tested with gcc and gnu binutils
from the
RISC-V project's 
`GNU compiler toolchain <https://github.com/riscv/riscv-gnu-toolchain>`__
repository.

The build process
will produce the file ``xinu.elf`` in the ``compile/`` directory.

Running
-------

::

    $ /usr/local/bin/qemu-system-riscv64 -nographic  -machine virt -kernel xinu.elf 

This is tested with QEMU QEMU emulator version 3.1.0 (v3.1.0-rc1-207-g3cc4afdb71-dirty)
,commit 3cc4afdb71 from the RISC-V project
`qemu git repository <https://github.com/riscv/riscv-qemu>`__.
Please consult this guide on building it from source:
`<https://github.com/riscv/riscv-qemu/wiki>`__.
The linux distribution provided QEMU versions may or may not work.


Bugs and Missing Features
-------------------------

The ``riscv64-qemu`` platform does not yet support networking.
