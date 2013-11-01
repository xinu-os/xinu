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

The two important functions associated with monitors are ``lock()`` and
``unlock()``. Full C code for these functions is provided below.
It is important to notice that these functions themselves must be
"synchronized" to ensure correctness. In the code provided we have
disabled interrupts, but a more lightweight synchronization mechanism
could also be used.

``lock.c`` (note: see :source:`system/lock.c` for latest development version):

::

    #include <thread.h>
    #include <semaphore.h>
    #include <monitor.h>

    /**
     * Current thread attempts to grab the lock on a monitor.
     * If another thread does not hold the lock the current thread owns the lock
     * and increases the monitor's count by one. Otherwise, the thread waits.
     * @param mon  target monitor
     * @return OK on success, SYSERR on failure
     */
    syscall lock(monitor mon)
    {
        register struct sement *semptr;
        register struct monent *monptr;
        irqmask im;

        im = disable();
        if (isbadmon(mon))
        {
            restore(im);
            return SYSERR;
        }

        monptr = &montab[mon];
        semptr = &semtab[monptr->sem];

        /* if no thread owns the lock, the current thread claims it */
        if (NOOWNER == monptr->owner)
        {
            monptr->owner = thrcurrent; /* current thread now owns the lock    */
            (monptr->count)++;          /* add 1 "lock" to the monitor's count */
            wait(monptr->sem);          /* this thread owns the semaphore      */
        }
        else
        {
            /* if current thread owns the lock increase count; dont wait on sem */
            if (thrcurrent == monptr->owner)
            {
                (monptr->count)++;
            }
            /* if another thread owns the lock, wait on sem until monitor is free */
            else
            {
                wait(monptr->sem);
                monptr->owner = thrcurrent;
                (monptr->count)++;

            }
        }

        restore(im);
        return OK;
    }

``unlock.c`` (note: see :source:`system/unlock.c` for latest development version):

::

    #include <thread.h>
    #include <semaphore.h>
    #include <monitor.h>

    /**
     * unlock a monitor. If monitor's count is 0, then free the monitor
     * and signal the monitor's semaphore.
     * @param mon  target monitor
     * @return OK on success, SYSERR on failure
     */
    syscall unlock(monitor mon)
    {
        register struct sement *semptr;
        register struct monent *monptr;
        irqmask im;

        im = disable();
        if (isbadmon(mon))
        {
            restore(im);
            return SYSERR;
        }

        monptr = &montab[mon];
        semptr = &semtab[monptr->sem];

        /* safety check: this unlock call does not have an associated lock call */
        if (monptr->count <= 0)
        {
            restore(im);
            return SYSERR;
        }

        /* decrement the monitor's count signifying one "unlock" */
        (monptr->count)--;

        /* if this is the top-level unlock call, then free this monitor's lock */
        if (monptr->count == 0)
        {
            monptr->owner = NOOWNER;
            signal(monptr->sem);
        }

        restore(im);
        return OK;
    }

