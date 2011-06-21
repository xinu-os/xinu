/**
 * @file queinit.c
 * @provides queinit.
 *
 * $Id: queinit.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <queue.h>

/**
 * Initialize a new queue in the global thread queue table
 * @return newly allocated queue ID or SYSERR
 */
qid_typ queinit(void)
{
    static int nextqid = NTHREAD;

                                          /**< next available quetab entry   */
    qid_typ q;

    if (nextqid > NQENT)
    {
        return SYSERR;
    }
    q = nextqid;
    nextqid += 2;
    quetab[quehead(q)].next = quetail(q);
    quetab[quehead(q)].prev = EMPTY;
    quetab[quehead(q)].key = MAXKEY;
    quetab[quetail(q)].next = EMPTY;
    quetab[quetail(q)].prev = quehead(q);
    quetab[quetail(q)].key = MINKEY;
    return q;
}
