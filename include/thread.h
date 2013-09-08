/**
 * @file thread.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _THREAD_H_
#define _THREAD_H_

#ifndef __ASSEMBLER__
#include <interrupt.h>
#include <semaphore.h>
#include <debug.h>
#include <stddef.h>
#include <memory.h>
#endif /* __ASSEMBLER__ */

/* unusual value marks the top of the thread stack                      */
#define STACKMAGIC  0x0A0AAAA9

/* thread state constants                                               */
#define THRCURR     1           /**< thread is currently running        */
#define THRFREE     2           /**< thread slot is free                */
#define THRREADY    3           /**< thread is on ready queue           */
#define THRRECV     4           /**< thread waiting for message         */
#define THRSLEEP    5           /**< thread is sleeping                 */
#define THRSUSP     6           /**< thread is suspended                */
#define THRWAIT     7           /**< thread is on semaphore queue       */
#define THRTMOUT    8           /**< thread is receiving with timeout   */
#define THRMIGRATE  9           /**< thread is being migrated           */

/* miscellaneous thread definitions                                     */
#define TNMLEN      16          /**< length of thread "name"            */
#define NULLTHREAD  0           /**< id of the null thread              */
#define BADTID      (-1)        /**< used when invalid tid needed       */

/* thread initialization constants */
#define INITSTK     65536       /**< initial thread stack size          */
#define INITPRIO    20          /**< initial thread priority            */
#define MINSTK      128         /**< minimum thread stack size          */
#ifdef JTAG_DEBUG
#define INITRET   debugret      /**< threads return address for debug   */
#else                           /* not JTAG_DEBUG */
#define INITRET   userret       /**< threads return address             */
#endif                          /* JTAG_DEBUG */

/* Reschedule constants for ready  */
#define RESCHED_YES 1           /**< tell ready to reschedule           */
#define RESCHED_NO  0           /**< tell ready not to reschedule       */

/* Check for invalid thread ids.  Note that interrupts must be disabled */
/* for the condition to hold true between statements.                   */
#define isbadtid(x) ((x)>=NTHREAD || (x)<0 || THRFREE == thrtab[(x)].state)

/** Maximum number of file descriptors a thread can hold */
#define NDESC       5

/** Maximum number of local devices */
#define NLOCDEV     10

/* Expose sizeof(struct thrent) and offsetof(struct thrent, stkdiv) to 
 * assembly files. */
#define THRENTSIZE 148
#define STKDIVOFFSET 104

#ifndef __ASSEMBLER__

/**
 * Defines what an entry in the thread table looks like.
 */
struct thrent
{
    uchar state;                /**< thread state: THRCURR, etc.        */
    int prio;                   /**< thread priority                    */
    void *stkptr;               /**< saved stack pointer                */
    void *stkbase;              /**< base of run time stack             */
    ulong stklen;               /**< stack length in bytes              */
    char name[TNMLEN];          /**< thread name                        */
    irqmask intmask;            /**< saved interrupt mask               */
    semaphore sem;              /**< semaphore waiting for              */
    tid_typ parent;             /**< tid for the parent thread          */
    message msg;                /**< message sent to this thread        */
    bool hasmsg;                /**< nonzero iff msg is valid           */
    struct memblock memlist;    /**< free memory list of thread         */
    int fdesc[NDESC];           /**< device descriptors for thread      */
};

extern struct thrent thrtab[];
extern int thrcount;            /**< currently active threads           */
extern tid_typ thrcurrent;      /**< currently executing thread         */

/* Inter-Thread Communication prototypes */
syscall send(tid_typ, message);
message receive(void);
message recvclr(void);
message recvtime(int);

/* Thread management function prototypes */

tid_typ create(void *procaddr, uint ssize, int priority,
               const char *name, int nargs, ...);
tid_typ gettid(void);
syscall getprio(tid_typ);
syscall kill(int);
int ready(tid_typ, bool);
int resched(void);
syscall sleep(uint);
syscall unsleep(tid_typ);
syscall yield(void);

/**
 * @ingroup threads
 *
 * Enter some kind of powerdown state (if it exists) that suspends
 * execution until an interrupt is detected.
 */
void pause(void);

void userret(void);

#endif /* __ASSEMBLER__ */

#endif                          /* _THREAD_H_ */
