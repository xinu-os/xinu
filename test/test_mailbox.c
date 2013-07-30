#include <stddef.h>
#include <mailbox.h>
#include <stdio.h>
#include <stdlib.h>
#include <testsuite.h>
#include <limits.h>
#include <interrupt.h>
#include <thread.h>

/* function prototypes */
static int producer(mailbox);
static int consumer(mailbox);

thread test_mailbox(bool verbose)
{
    bool passed = TRUE;
    struct mbox *pmbox;
    mailbox testbox1;
    mailbox testbox2;
    int i = 0;
    int prio = 0;
    int count = 0;
    tid_typ consumertid;
    tid_typ producertid;
    struct thrent *thrptr;
    mailbox boxes[NMAILBOX];

    enable();

    /* Test allocation of mailboxes */
    testPrint(verbose, "Allocate small mailbox");

    testbox1 = mailboxAlloc(1);
    pmbox = &mboxtab[testbox1];

    if (SYSERR == (uint)testbox1)
    {
        passed = FALSE;
        testFail(verbose, "allocating small mailbox results in SYSERR");
    }
    else
    {
        if (pmbox->max != 1)
        {
            passed = FALSE;
            testFail(verbose,
                     "does not allocate correct count of messages");
        }
        else
        {
            testPass(verbose, "");
        }
    }

    testPrint(verbose, "Allocate large mailbox");

    testbox2 = mailboxAlloc(384);
    pmbox = &mboxtab[testbox2];

    if (SYSERR == (uint)testbox2)
    {
        passed = FALSE;
        testFail(verbose, "allocating large mailbox results in SYSERR");
    }
    else
    {
        if (pmbox->max != 384)
        {
            passed = FALSE;
            testFail(verbose,
                     "does not allocate correct count of messages");
        }
        else
        {
            testPass(verbose, "");
        }
    }

    /* Test de-allocation (freeing) of mailboxes */

    testPrint(verbose, "Free mailboxes");

    if (SYSERR == mailboxFree(testbox1))
    {
        passed = FALSE;
        testFail(verbose, "freeing mailbox results in SYSERR");
    }
    else
    {
        if (SYSERR == mailboxFree(testbox2))
        {
            passed = FALSE;
            testFail(verbose, "freeing mailbox results in SYSERR");
        }
        else
        {
            testPass(verbose, "");
        }
    }

    /* Test allocation of many mailboxes */

    testPrint(verbose, "Allocate all mailboxes");
    count = 0;
    for (i = 0; i < NMAILBOX + 1; i++)
    {
        boxes[count] = mailboxAlloc(1);
        if (SYSERR == boxes[count])
        {
            break;
        }
        count++;
    }
    failif((NMAILBOX + 1 == i), "allocated too many");

    /* Clean up allocated mailboxes */

    for (i = 0; i < count; i++)
    {
        if (SYSERR == mailboxFree(boxes[i]))
        {
            passed = FALSE;
            testFail(verbose, "freeing mailbox results in SYSERR");
        }
    }

    /* Test basic sending */

    testPrint(verbose, "Sending messages");

    testbox1 = mailboxAlloc(3);

    if ((SYSERR == mailboxSend(testbox1, 1))
        || (SYSERR == mailboxSend(testbox1, 2))
        || (SYSERR == mailboxSend(testbox1, 3)))
    {
        passed = FALSE;
        testFail(verbose, "failed sending mail-message to mailbox");
    }
    else
    {
        testPass(verbose, "");
    }

    /* Test basic receiving */

    testPrint(verbose, "Receiving messages");

    if ((1 != mailboxReceive(testbox1))
        || (2 != mailboxReceive(testbox1))
        || (3 != mailboxReceive(testbox1)))
    {
        passed = FALSE;
        testFail(verbose, "failed receiving mail-message from mailbox");
    }
    else
    {
        testPass(verbose, "");
    }

    /* Free mailbox */

    mailboxFree(testbox1);

    /* Test consumer waiting on empty mailbox */

    testPrint(verbose, "Wait on empty mailbox");

    testbox1 = mailboxAlloc(3);

    prio = getprio(gettid());

    consumertid =
        create((void *)consumer, INITSTK, prio + 1, "consumer", 1,
               testbox1);
    ready(consumertid, RESCHED_YES);

    thrptr = &thrtab[consumertid];

    if (thrptr->state != THRWAIT)
    {
        passed = FALSE;
        testFail(verbose, "consumer did not wait on empty mailbox");
    }
    else
    {
        testPass(verbose, "");
    }

    /* Kill waiting consumer thread */

    kill(consumertid);

    /* Test producer waiting on full mailbox */

    testPrint(verbose, "Wait on full mailbox");

    producertid =
        create((void *)producer, INITSTK, prio + 1, "producer", 1,
               testbox1);

    ready(producertid, RESCHED_YES);

    thrptr = &thrtab[producertid];

    if (thrptr->state != THRWAIT)
    {
        passed = FALSE;
        testFail(verbose, "producer did not wait on full mailbox");
    }
    else
    {
        testPass(verbose, "");
    }

    /* Kill waiting producer thread */

    kill(producertid);

    /* Free mailbox */

    mailboxFree(testbox1);

    /* Final report */
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

thread producer(mailbox box)
{
    int i = 0;
    for (i = 0; i < 4; i++)
    {
        mailboxSend(box, i);
    }

    return OK;
}

thread consumer(mailbox box)
{
    int i = 0;

    for (i = 0; i < 3; i++)
    {
        mailboxReceive(box);
    }

    return OK;
}
