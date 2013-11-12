Memory
======

Memory on MIPS-based processors is broken into several segments,
consuming the entire 32-address space. These segments are arranged as
follows:

-  `User Segment`_ (USEG), 2 GB **mapped** and
   **cached**, addresses ``0x0000 0000`` through ``0x7FFF FFFF``
-  `Kernel Segment 0`_ (KSEG0), 512 MB **unmapped** and **cached**,
   addresses ``0x8000 0000`` through ``0x9FFF FFFF``
-  `Kernel Segment 1`_ (KSEG1), 512 MB **unmapped** and **uncached**,
   addresses ``0xA000 0000`` through ``0xBFFF FFFF``
-  `Kernel Segment 2`_ (KSEG2), 1 GB **mapped** and **cached**,
   addresses ``0xC000 0000`` through ``0xFFFF FFFF``

Note that the :doc:`WRT54GL` only has 16 MB of main memory, so a 1-1
mapping is not be available above ``0x..FF FFFF``.

.. _mips_user_segment:

User Segment
------------

The user segment of memory, or USEG, is the range of memory addresses
from ``0x0000 0000`` through ``0x7FFF FFFF``. This memory is both mapped
and cached, meaning that any attempt to access memory within this range
will result in the hardware consulting the memory manager prior to
access. **Note** that any attempted access in this range must with the
CPU privilege level set to user mode (i.e. :math:`Status(KSU) = 2`) or
the processor must be in the exception state with the error level bit
set (i.e. :math:`Status(EXL) = 1`). In the latter case, all mappings
turn into 1-1, so directly accessing these address is not recommended.

Under normal operation, when an address in this memory range is
accessed, the processor will query the memory management unit is
consulted for a mapping. In turn this will ask the translation lookaside
buffer (TLB), and if no mapping is found cause a TLB load or store
exception. When this exception occurs, the operating system is consulted
and should write the correct mapping to the TLB and return from the
exception handler.

It should be noted that when a TLB exception occurs in the USEG range of
addresses, a special "fast" exception handler is consulted instead of
the normal exception handler. This fast handler is at most 32
instructions long and begins at ``0x8000 0000``.

User Segment under Xinu
~~~~~~~~~~~~~~~~~~~~~~~

Embedded Xinu has basic :doc:`memory management
</features/Memory-Management>` as of the 2.0 release.  During
initialization, the kernel allocates some amount of memory (defined in
``xinu.conf`` as ``UHEAP_SIZE``) to act as the user memory heap. Once
this memory is initialized, calls to ``malloc`` and ``free`` will use
the user heap for memory allocation and automatically insert mappings
into the system page table. All mappings are 1-1 since there is no
backing store for a virtual memory subsystem, though it would be
possible to provide each thread with a private address space this
requires lots of memory overhead.

Exception code to handle TLB faults is in the
:source:`tlbMissHandler() <mem/tlbMissHandler.c>` function and simply
performs a lookup of the faulting address in the system page table,
checks to see if it is valid and in the correct address space, and
inserts the mapping in the TLB hardware.

Kernel Segments
---------------

While the first part of memory is dedicated to the user segment, the
remainder is the kernel segment. Unlike the user segment, the kernel
segment is sub-divided into three segments with different memory access
properties. **Note** that these properties can be very important if you
are dealing with device drivers and/or hardware that uses direct memory
access (DMA) because there may be caching considerations.

In brief:

-  `KSEG0 <Kernel Segment 0>`_ uses **unmapped** and **cached** memory
   accesses,
-  `KSEG1 <Kernel Segment 1>`_ uses **unmapped** and **uncached**
   memory accesses, and
-  `KSEG2 <Kernel Segment 2>`_ uses **mapped** and **cached** memory
   accesses.

Kernel Segment 0
~~~~~~~~~~~~~~~~

The first kernel segment, or KSEG0, is the range of memory addresses
from ``0x8000 0000`` through ``0x9FFF FFFF``. This memory is
**unmapped** and **cached**, meaning that when the processor attempts
to access an address in this range it will not consult the memory
manager for a mapping, but will store and modifications in the on-chip
memory cache.  **Note** that when allocating memory for a device
driver that will be using DMA, the caching effects could lead to major
headaches. If memory is being given to a hardware backend it should be
mapped into the range of `KSEG1 <Kernel Segment 1>`_.

On many MIPS processors the first page (4096 bytes) of KSEG0 is
considered to be reserved system space where small amounts of
specialized code can be loaded for fast execution. Typically, this
memory is used for exception handling and the following sections are
reserved for the following:

-  ``0x8000 0000`` (32 instructions) is for the TLB exception handler,
-  ``0x8000 0080`` (32 instructions) is for the 64-bit TLB exception
   handler, and
-  ``0x8000 0180`` (32 instructions) is for the generic exception
   handler.

Other reserved portions of this memory page remain unknown.

After the reserved system page, the operating system is free to use
memory however it sees fit.

KSEG0 under Xinu
^^^^^^^^^^^^^^^^

Embedded Xinu uses KSEG0 extensively for kernel operations. As the
:doc:`WRT54GL` uses a 32-bit MIPS processor, Embedded Xinu loads a
quick TLB handler into memory at ``0x8000 0000`` and a generic
exception handler at ``0x8000 0180``, both of these are limited to 32
instructions and jump to higher level C code when needed. It is
important to note that Embedded Xinu also makes use of reserved memory
starting at ``0x8000 0200`` to store an array of exception handler
entry points (32-bit function pointers for 32 possible exceptions) and
``0x8000 0280`` to store an array of interrupt handler entry points
(32-bit function pointers for 8 possible interrupts).

Xinu loads the kernel entry point beginning at ``0x8000 1000``, and upon
booting begins execution at that address. Within Embedded Xinu, the
memory segments are in the following order (as defined by
``ld.script``): text, read-only data, data, and block started by symbol
(BSS, uninitialized data). The kernel allocates a small kernel stack
after the BSS segment and finally initializes a dynamic memory heap for
the remaining physical memory addresses.

The :doc:`kernel memory allocator </features/Memory-Management>` will
allocate memory from the kernel heap as requested. Since Embedded Xinu
uses a single page table, all kernel addresses will be mapped
read-only in all address spaces, giving a user thread the ability to
read from but not write to kernel memory.

Kernel Segment 1
~~~~~~~~~~~~~~~~

The second kernel segment, or KSEG1, is the range of memory addresses
from ``0xA000 0000`` through ``0xBFFF FFFF``. This memory **unmapped**
and **uncached**, meaning that when the processor attempts to access
an address in this range it will not consult the memory manager for a
mapping and it *will* bypass the on-chip memory cache for memory loads
and stores.

By skipping the hardware cache, KSEG1 will see slower memory accesses
because it must get data directly from the RAM. Because of this, it is
not typical to use KSEG1 for normal kernel operations, rather this
segment is useful for accessing memory that is mapped to some other
hardware device on the platform. These mappings will either be
pre-existing, so they are out-of-range of physical memory addresses, or
they will be dynamically allocated memory that will be shared between
the operating system and some hardware device.

KSEG1 under Xinu
^^^^^^^^^^^^^^^^

Embedded Xinu uses several hardware devices that are mapped
out-of-range of physical memory and some hardware devices that use
dynamically allocated memory for sharing. Some devices on the
:doc:`WRT54GL` that are beyond the range of physical memory are:

-  Broadcom I/O controller registers at ``0xB800 0000``,
-  UART registers at ``0xB800 0300`` and ``0xB800 0400``,
-  Broadcom Ethernet 47xx registers at ``0xB800 1000``,
-  Broadcom Wireless LAN controller registers at ``0xB800 5000``,
-  Broadcom 47xx RoboSwitch registers at ``0xB800 6000``, and
-  :doc:`Flash memory <Flash-Memory>` (4 MB) read mapped beginning at
   ``0xBC00 0000``.

Certain drivers (such as the Ethernet driver), also take advantage of
shared memory between the operating system and the hardware. This
requires the use of dynamically allocated kernel memory (originating in
KSEG0), that has been mapped to KSEG1 address range. This is not
problematic because both KSEG0 and KSEG1 use a 1-1 memory mapping. With
the Ethernet driver of Embedded Xinu, the shared memory that is in KSEG1
hold the DMA descriptor rings and the Ethernet packet buffers to store
the packets in.

Kernel Segment 2
~~~~~~~~~~~~~~~~

The third kernel segment, or KSEG2, is the range of memory addresses
from ``0xC000 0000`` through ``0xFFFF FFFF``. This memory is both
**mapped** and **cached**, meaning that the processor will consult the
memory manager for a mapping and store memory modifications in the
on-chip cache.

Like the user segment of memory any attempt to access memory in KSEG2
will result in the processor querying the memory manager and the TLB to
find a mapping. If a mapping does not exist the processor will generate
a TLB load or store exception and the operating system must fill the TLB
entry. Unlike USEG, a TLB exception will not jump to the "fast" handler
and instead follow the normal path for exception handling through the
generic exception mechanism.

This memory segment could be useful to create the appearance of page
aligned data to the underlying hardware or operating system if needed.

KSEG2 under Xinu
^^^^^^^^^^^^^^^^

Embedded Xinu does not make use of any KSEG2 memory yet. However, to
take advantage of the Context register of MIPS processors when a TLB
exception occurs, it is possible that a mapping of the system page table
to KSEG2 might exist in future versions.

References
----------

* Sweetman, Dominic. *See MIPS Run*. San Francisco: Morgan Kaufmann
  Publishers, 2007.
