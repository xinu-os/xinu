BCM2835 memory barriers
=======================

As documented on page 7 of the *BCM2835 ARM Peripherals* document
[#bcm]_, ARM code executing on the :doc:`BCM2835` in the
:doc:`Raspberry-Pi` should:

- execute a memory write barrier before the first write to a peripheral
- execute a memory read barrier after the last read to a peripheral

As of this writing, in Embedded Xinu both types of memory barrier are
implemented by the same :source:`memory_barrier()
<system/arch/arm/memory_barrier.S>` function, which executes a **data
memory barrier**, which is an operation that ensures that all explicit
memory accesses occurring previously in program order are globally
observed before any explicit memory accesses occurring subsequently in
program order.  This should be sufficient to prevent problems with
memory access reordering on the BCM2835.

``memory_barrier()`` is already called before and after interrupt
dispatch, but any other code that attempts to access peripherals
outside of the interrupt handler, or access multiple peripherals
within the some interrupt handler, must make its own call to
``memory_barrier()``.  This does not need to occur before and after
every access to the peripheral, only after a series of accesses to a
*single peripheral*.

Despite the above, we have not encountered any observable difference
when no memory barriers are used, so their true necessity on the
BCM2835 is unclear.

Notes
-----

.. [#bcm] http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
