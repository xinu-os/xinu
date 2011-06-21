#include <stddef.h>
#include <thread.h>
#include <queue.h>
#include <semaphore.h>
#include <stdio.h>
#include <testsuite.h>

thread test_deltaQueue(bool verbose)
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

    q = semtab[s].queue;

    /* Create some valid threads IDs to stuff in the queue.  */
    /* These threads had better not ever run. */
    atid = create((void *)NULL, INITSTK, 0, "DELTA-A", 0);
    btid = create((void *)NULL, INITSTK, 0, "DELTA-B", 0);
    ctid = create((void *)NULL, INITSTK, 0, "DELTA-C", 0);
    dtid = create((void *)NULL, INITSTK, 0, "DELTA-D", 0);

    if ((SYSERR == atid) || (SYSERR == btid)
        || (SYSERR == ctid) || (SYSERR == dtid))
    {
        passed = FALSE;
    }

    testPrint(verbose, "Delta sorting");
    insertd(btid, q, 5);
    insertd(ctid, q, 6);
    insertd(atid, q, 4);
    insertd(dtid, q, 7);

    if ((4 == firstkey(q)) && (atid = dequeue(q))
        && (1 == firstkey(q)) && (btid = dequeue(q))
        && (1 == firstkey(q)) && (ctid = dequeue(q))
        && (1 == firstkey(q)) && (dtid = dequeue(q)))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
        testFail(verbose, "insertd out of order");
    }

    while (EMPTY != dequeue(q))
        ;

    testPrint(verbose, "Same delta");
    insertd(atid, q, 5);
    insertd(btid, q, 5);
    insertd(ctid, q, 5);
    insertd(dtid, q, 5);

    if ((5 == firstkey(q)) && (atid = dequeue(q))
        && (0 == firstkey(q)) && (btid = dequeue(q))
        && (0 == firstkey(q)) && (ctid = dequeue(q))
        && (0 == firstkey(q)) && (dtid = dequeue(q)))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
        testFail(verbose, "insertd out of order");
    }

    while (EMPTY != dequeue(q))
        ;

    kill(atid);
    kill(btid);
    kill(ctid);
    kill(dtid);

    semfree(s);

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
