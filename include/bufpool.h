/**
 * @file bufpool.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _BUFPOOL_H_
#define _BUFPOOL_H_

#include <stddef.h>
#include <semaphore.h>
#include <conf.h>

/* Buffer pool state definitions */
#define BFPFREE  1
#define BFPUSED  2

/**
 * Individual pool buffer
 */
struct poolbuf
{
    struct poolbuf *next;
    int poolid;
};

/**
 * Buffer pool table entry
 */
struct bfpentry
{
    uchar state;
    uint bufsize;
    uint nbuf;
    void *head;
    struct poolbuf *next;
    semaphore freebuf;
};

/**
 * isbadpool - check validity of reqested buffer pool id and state
 * @param p id number to test
 */
#define isbadpool(p) ((p >= NPOOL)||(p < 0)||(BFPFREE == bfptab[p].state))

/** roundword - round byte sized request to word size
 *  @param b size in bytes
 */
#define roundword(b) ((3 + b) & ~0x03)

extern struct bfpentry bfptab[];

/* function prototypes */
void *bufget(int);
syscall buffree(void *);
int bfpalloc(uint, uint);
syscall bfpfree(int);

#ifndef NPOOL
#  define NPOOL 0
#endif

/* Fill in dummy definitions if no pools are defined  */
#if NPOOL == 0
#  ifndef POOL_MAX_BUFSIZE
#    define POOL_MAX_BUFSIZE 0
#  endif
#  ifndef POOL_MIN_BUFSIZE
#    define POOL_MIN_BUFSIZE 0
#  endif
#  ifndef POOL_MAX_NBUFS
#    define POOL_MAX_NBUFS 0
#  endif
#endif

#endif                          /* _BUFPOOL_H_ */
