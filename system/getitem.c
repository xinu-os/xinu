/**
 * @file getitem.c
 * @provides getfirst, getlast, getitem.
 *
 * $Id: getitem.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <queue.h>

/**
 * Remove a thread from front of queue
 * @param  q  target queue
 * @return thread ID of removed thread
 */
tid_typ getfirst(qid_typ q)
{
    tid_typ head;

    if (isbadqid(q))
    {
        return SYSERR;
    }
    if (isempty(q))
    {
        return EMPTY;
    }

    head = quehead(q);
    return getitem(quetab[head].next);
}

/**
 * Remove a thread from end of queue
 * @param  q  target queue
 * @return thread ID of removed thread
 */
tid_typ getlast(qid_typ q)
{
    tid_typ tail;

    if (isbadqid(q))
    {
        return SYSERR;
    }
    if (isempty(q))
    {
        return EMPTY;
    }

    tail = quetail(q);
    return getitem(quetab[tail].prev);
}

/**
 * Remove a thread from anywhere in a queue
 * @param  tid  thread ID to get
 * @return thread ID of removed thread
 */
tid_typ getitem(tid_typ tid)
{
    tid_typ prev, next;

    next = quetab[tid].next;
    prev = quetab[tid].prev;
    quetab[prev].next = next;
    quetab[next].prev = prev;
    quetab[tid].next = EMPTY;
    quetab[tid].prev = EMPTY;
    return tid;
}
