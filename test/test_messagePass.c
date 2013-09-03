#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread.h>
#include <testsuite.h>
#include <clock.h>
#include <interrupt.h>
#include <thread.h>

static thread recvthread(bool);

/* test_messagePass -- Creates two threads; a receiver and a sender.  
 * Each testing send, receive, receive clear, and receive timeout. 
 * Called by xsh_testsuite()
 */
thread test_messagePass(bool verbose)
{
    bool passed = TRUE;
    tid_typ recvtid;

    recvclr();

    /* Sending messages */
    testPrint(verbose, "Send message");
    if (OK == send(gettid(), 'a'))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
        testFail(verbose, "");
    }

    /* Sending multiple messages */
    testPrint(verbose, "Send multiple messages");
    if (SYSERR == send(gettid(), 'd'))
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
        testFail(verbose,
                 "\nsend() returned OK even though a message already exists");
    }

    /* Receive message */
    testPrint(verbose, "Receive waiting message");
    if ('a' == receive())
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
        testFail(verbose, "");
    }

    /* Receive clear - message pending */
    if (OK == send(gettid(), 'b'))
    {
        testPrint(verbose, "Receive clear - message pending");
        if ('b' == recvclr())
        {
            testPass(verbose, "");
        }
        else
        {
            passed = FALSE;
            testFail(verbose, "");
        }
    }
    else
    {
        passed = FALSE;
        testFail(verbose, "\nunable to send message");
    }

    /* Receive clear - no pending message */
    testPrint(verbose, "Receive clear - no message pending");
    if (NOMSG == recvclr())
    {
        testPass(verbose, "");
    }
    else
    {
        passed = FALSE;
        testFail(verbose, "");
    }


    /* Receive timeout */
    testPrint(verbose, "Receive timeout");
    recvtid = create((void *)recvthread, INITSTK, getprio(gettid()),
                     "recvthread", 1, verbose);
    if ((SYSERR == recvtid) || (SYSERR == ready(recvtid, RESCHED_YES)))
    {
        passed = FALSE;
        testFail(verbose, "\nunable to create receiver thread");
    }
    else
    {
        sleep(100);
        if (THRRECV == thrtab[recvtid].state)
        {
            kill(recvtid);
            passed = FALSE;
            testFail(verbose, "\nreceive thread still waiting");
        }

        if (THRSLEEP == thrtab[recvtid].state)
        {
            kill(recvtid);
            passed = FALSE;
        }
    }

    if (passed)
    {
        testPass(TRUE, "");
    }
    else
    {
        testFail(TRUE, "");
    }

    return OK;
}

static thread recvthread(bool verbose)
{
    if (TIMEOUT == recvtime(1))
    {
        testPass(verbose, "");
        return OK;
    }
    else
    {
        testFail(verbose, "\nrecvtime() did not return TIMEOUT");
        sleep(10000);
    }

    return SYSERR;
}
