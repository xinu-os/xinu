
/**
 * @file monitor.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _MONITOR_H_
#define _MONITOR_H_

#include <thread.h>
#include <semaphore.h>

#ifndef NMON
#define NMON 0
#endif

/* Monitor state definitions */
#define MFREE 0x01 /**< this monitor is free */
#define MUSED 0x02 /**< this monitor is used */

#define NOOWNER (-1) /**< no thread owns this monitor's lock */

/* type definition of "monitor" */
typedef unsigned int monitor;

/**
 * Monitor table entry
 */
struct monent                   /* monitor table entry */
{
    char state;         /**< the state MFREE or MUSED */
    tid_typ owner;              /**< the thread that owns the lock, or 0 if unowned */
    int count;                          /**< the number of lock actions performed */
    semaphore sem;              /**< the semaphore used by this monitor */
};

extern struct monent montab[];

/* isbadmon - check validity of reqested monitor id and state */
#define isbadmon(m) ((m >= NMON) || (MFREE == montab[m].state))

/* Monitor function prototypes */
syscall lock(monitor);
syscall unlock(monitor);
monitor moncreate(int);
syscall monfree(monitor);
syscall moncount(monitor);

//testing
thread monitortest(void);

#endif                          /* _MONITOR_H */
