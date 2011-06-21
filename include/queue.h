/**
 * @file queue.h 
 * @provides firstid, firstkey, isempty, lastkey, nonempty.
 * 
 * The thread queue system allows a statically-allocated array to
 * model sorted thread queueing before more complex operating system
 * services, like dynamic memory allocation, are up and running.
 *
 * These thread queues assume several important invariants:
 *  1) A fixed number of thread queues in the system, known
 *     at compile time,
 *  2) A thread queue can contain only threads and their
 *     associated key values, and thus queue length is at
 *     most NTHREAD, the total number of threads, and
 *  3) A thread can be in at most one queue at a time.
 *
 * Ordering of threads within a given queue depends upon the sorting
 * functions called when maintaining that particular system queue.
 *
 * $Id: queue.h 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

/* queue structure declarations, constants, and inline procedures       */

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <kernel.h>

#ifndef NQENT

/** NQENT = 1 per thread, 2 per list, 2 per sem */
# define NQENT   (NTHREAD + 4 + NSEM + NSEM)
#endif

#define EMPTY (-2)              /**< null pointer for queues            */
#define MAXKEY 0x7FFFFFFF       /**< max key that can be saved in queue */
#define MINKEY 0x80000000       /**< min key that can be saved in queue */

typedef int qid_typ;            /**< represent queue id by head index   */

/**
 * Defines what an entry in the queue table looks like.
 */
struct queent
{
    int key;                    /**< key on which the queue is ordered  */
    tid_typ next;               /**< index of next thread or tail       */
    tid_typ prev;               /**< index of previous thread or head   */
};

extern struct queent quetab[];
extern qid_typ readylist;

#define quehead(q) (q)
#define quetail(q) ((q) + 1)

/* Check for invalid queue ids.  Note that interrupts must be disabled  */
/* for the condition to hold true between statements.                   */
#define isbadqid(x)  ((quehead(x) < 0) || \
                      (quehead(x) != (quetail(x) - 1)) || \
                      (quetail(x) >= NQENT))

#define isempty(q)   (quetab[quehead(q)].next >= NTHREAD)
#define nonempty(q)  (quetab[quehead(q)].next <  NTHREAD)
#define firstkey(q)  (quetab[quetab[quehead(q)].next].key)
#define lastkey(q)   (quetab[quetab[quetail(q)].prev].key)
#define firstid(q)   (quetab[quehead(q)].next)

/* Queue function prototypes */
tid_typ getfirst(qid_typ);
tid_typ getlast(qid_typ);
tid_typ getitem(tid_typ);
tid_typ enqueue(tid_typ, qid_typ);
tid_typ dequeue(qid_typ);
int insert(tid_typ, qid_typ, int);
int insertd(tid_typ, qid_typ, int);
qid_typ queinit(void);

#endif                          /* _QUEUE_H_ */
