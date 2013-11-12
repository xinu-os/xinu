Monitors
========

What are Monitors?
------------------

Java monitors act as locks guarding fields and methods of an object.
Each Java object is associated with one monitor. The *synchronized*
keyword requires a thread to acquire the monitor lock associated with a
synchronized method’s target object before executing the body. Two
different synchronized methods belonging to the same object both depend
on the same lock as the monitor is associated with the object, and not
the methods.

A thread which has already acquired a lock does not wait when attempting
to acquire that same lock – this is the primary difference between
Java-style monitors and typical counting semaphores. Any thread holding
a lock must perform an unlock action once for each corresponding lock
action before releasing the lock.

Monitors in Xinu
----------------

The monitors we add to Xinu contain an associated semaphore, a thread
ownership ID, and a count tracking the number of locks performed without
corresponding unlocks. Thus, a monitor count begins at zero, every
successful lock action increases the count by one, and every unlock
action decreases the count by one. They are handled by a monitor table
similar to other structure tables in Xinu such as the mailbox table,
semaphore table, and thread table.

The two important functions associated with monitors are ``lock()``
and ``unlock()``. Full C code for these functions can be found in
:source:`system/lock.c` and :source:`system/unlock.c`.  It is
important to notice that these functions themselves must be
"synchronized" to ensure correctness.  In the code we have disabled
interrupts, but a more lightweight synchronization mechanism could
also be used.
