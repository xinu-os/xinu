#include <stddef.h>
#include <thread.h>
#include <queue.h>
#include <semaphore.h>
#include <stdio.h>
#include <testsuite.h>

thread test_procQueue(bool verbose)
{
    semaphore s;
    qid_typ q;
    bool passed = TRUE;
    tid_typ atid, btid, ctid, dtid;
    char msg[50];

    /* Create a semaphore so that we can use its queue. */
    s = semcreate(0);

    if (isbadsem(s))
    {
        passed = FALSE;
        sprintf(msg, "No semaphore, %d", s);
        testFail(verbose, msg);
    }
    else
    {
        q = semtab[s].queue;

        /* Create some valid threads IDs to stuff in the queue.  */
        /* These threads had better not ever run. */
        atid = create((void *)NULL, INITSTK, 0, "PQUEUE-A", 0);
        btid = create((void *)NULL, INITSTK, 0, "PQUEUE-B", 0);
        ctid = create((void *)NULL, INITSTK, 0, "PQUEUE-C", 0);
        dtid = create((void *)NULL, INITSTK, 0, "PQUEUE-D", 0);

        if ((SYSERR == atid) || (SYSERR == btid)
            || (SYSERR == ctid) || (SYSERR == dtid))
        {
            passed = FALSE;
            testFail(verbose, "Test process create error");
        }
        /* Thread create successful.  Insert into sem queue in   */
        /* priority order, and then insert with same priority.   */
        /* Use dequeue to verify proper insert order and key     */
        else
        {
            testPrint(verbose, "Priority insert sorting");
            insert(ctid, q, 5);
            insert(btid, q, 6);
            insert(dtid, q, 4);
            insert(atid, q, 7);

            if ((7 == firstkey(q))
                && (atid == dequeue(q))
                && (6 == firstkey(q))
                && (btid == dequeue(q))
                && (5 == firstkey(q))
                && (ctid == dequeue(q))
                && (4 == firstkey(q)) && (dtid == dequeue(q)))
            {
                testPass(verbose, "");
            }
            else
            {
                passed = FALSE;
                testFail(verbose, "insert out of order");
            }

            /* Empty queue for next test */
            while (EMPTY != dequeue(q))
                ;

            testPrint(verbose, "Same priority insert");
            insert(atid, q, 5);
            insert(btid, q, 5);
            insert(ctid, q, 5);
            insert(dtid, q, 5);

            if ((5 == firstkey(q))
                && (atid = dequeue(q))
                && (5 == firstkey(q))
                && (btid == dequeue(q))
                && (5 == firstkey(q))
                && (ctid == dequeue(q))
                && (5 == firstkey(q)) && (dtid == dequeue(q)))
            {
                testPass(verbose, "");
            }
            else
            {
                passed = FALSE;
                testFail(verbose, "insert out of order");
            }

            /* Empty queue for next test */
            while (EMPTY != dequeue(q))
                ;

            testPrint(verbose, "Dequeue using getfirst()");
            insert(ctid, q, 5);
            insert(btid, q, 6);
            insert(dtid, q, 4);
            insert(atid, q, 7);

            if (atid == getfirst(q))
            {
                testPass(verbose, "");
            }
            else
            {
                passed = FALSE;
                testFail(verbose, "");
            }

            /* Empty queue for next test */
            while (EMPTY != dequeue(q))
                ;

            testPrint(verbose, "Dequeue using getlast()");
            insert(ctid, q, 5);
            insert(btid, q, 6);
            insert(dtid, q, 4);
            insert(atid, q, 7);

            if (dtid == getlast(q))
            {
                testPass(verbose, "");
            }
            else
            {
                passed = FALSE;
                testFail(verbose, "");
            }

            /* Empty queue for next test */
            while (EMPTY != dequeue(q))
                ;

            kill(atid);
            kill(btid);
            kill(ctid);
            kill(dtid);
        }
        semfree(s);
    }

    if (TRUE == passed)
    {
        testPass(TRUE, "");
    }
    else
    {
        testFail(TRUE, "");
    }

    return OK;
}
