Memory management
=================

Memory management is an important aspect of any operating system. As
such, XINU makes use of some aspects of the underlying hardware to
build up a simple-to-understand memory management system.

Memory allocators
-----------------

XINU maintains two memory allocators that work in tandem to provide
dynamic memory to both kernel and user software. The first allocator
is the kernel allocator which allocates small chunks of memory from
the global memory heap as needed by the kernel. The second allocator
is a user allocator, that allocates memory from a per-thread memory
heap as needed by user processes.

Kernel allocator
~~~~~~~~~~~~~~~~

The most basic memory allocator in the system is the kernel allocator
which uses the ``memget`` and ``memfree`` functions. This operates on
the global kernel heap that uses the ``memlist`` global variable. In
this allocator the kernel developer is trusted to keep track of the
accounting information for memory blocks. This makes a rather
straightforward API.

.. code:: c

    void *memptr = memget(nbytes);
    memfree(memptr, nbytes);

As can be seen in the above API, the allocation function takes a single
parameter (``nbytes``) which is the number of bytes requested. The
deallocation function takes two parameters (``memptr`` and ``nbytes``),
where ``memptr`` is the memory address allocated via the ``memget``
function and ``nbytes`` is the number of bytes requested with the
original call.

User allocator
~~~~~~~~~~~~~~

Unlike the kernel allocator, the user allocator does not trust the
programmer to remember the amount of memory requested and instead stores
the accounting information immediately before the allocated memory. To
the programmer the API for user memory is simply:

.. code:: c

    void *memptr = malloc(nbytes);
    free(memptr);

This allocator works on a per-thread memory list of free memory, this
allows memory to be owned by the calling thread and prevents other
threads from having access to the memory. This forms the basis of memory
protection.

When a request for memory comes in to the allocator, it attempts to
satisfy the request with free memory that has already been allocated to
thread. If that fails, the allocator will then attempt to acquire memory
from the region allocator (described below). Since the region allocator
works at page granularity, any excess memory is inserted into the
thread's free memory list for future requests. When a block of memory is
free'd, the memory is returned to the thread's free memory list.

It is not until the thread is killed that the memory is removed from the
thread's protection domain and made available to the region allocator.

Region allocator
^^^^^^^^^^^^^^^^

The region allocator works beneath the user allocator and is
initialized during the boot process. During system boot XINU uses
``UHEAP_SIZE`` as defined in ``xinu.conf`` to allocate memory for the
user heap. This memory is allocated via the kernel ``memget()``
function and is then passed to the ``memRegionInit()`` function. Once
the region allocator is initialized, the only user level interface to
the region allocator is hidden behind the ``malloc`` and ``free``
routines.

Memory protection
-----------------

.. note::

   This section applies to MIPS platforms only.

Since XINU has limited resources to work with it does not provide a
virtual memory system. It does take advantage of separate address
spaces for each user thread running in the system, which provides
simple memory protection for low overhead costs. As such, when
allocating pages to the thread via the user allocator those pages will
be mapped to the protection domain of the currently running thread.
These protection domains are inserted into a single global page table,
that hold all the page table entries and the address space identifier
of the protected page.

In the memory protection subsystem, the default behaviour is to map all
the kernel pages (i.e. pages that are not in the user heap), to every
thread in the system as read only. This allows all threads to read from
kernel data, but prevents overwriting of that data.

Translation lookaside buffer
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. note::

   This section applies to MIPS platforms only.

To facilitate memory protection, XINU uses the translation lookaside
buffer (TLB) built into the MIPS processors of the WRT54GL series of
routers. When a piece of software attempts to access memory in the
:ref:`user segment <mips_user_segment>`, a TLB load or store exception
will occur. When this occurs the processor jumps to a specific
exception handler which allows the kernel to look up the page table
entry, check if the faulting thread is in the same memory space as the
entry, and load the entry into the TLB. If there is no mapping or the
thread is not in the same address space, a memory protection violation
occurs and the thread is killed.
