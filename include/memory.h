/**
 * @file memory.h
 * @provides roundmb, truncmb, stkfree.
 * Definitions for kernel memory allocator and maintenance.
 *
 * $Id: memory.h 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stddef.h>

/* roundmb - round address up to size of memblock  */
#define roundmb(x)  (void *)( (7 + (ulong)(x)) & ~0x07 )
/* truncmb - truncate address down to size of memblock */
#define truncmb(x)  (void *)( ((ulong)(x)) & ~0x07 )

/* stkfree - free the allocated stack memory */
#define stkfree(p, len) memfree((void *)((ulong)(p)         \
                                - (ulong)roundmb(len)       \
                                + (ulong)sizeof(ulong)),    \
                                (ulong)roundmb(len))


/**
 * Structure for a block of memory.
 */
struct memblock
{
    struct memblock *next;          /**< pointer to next memory block       */
    uint length;                    /**< size of memory block (with struct) */
};

extern struct memblock memlist;     /**< head of free memory list           */

/* Other memory data */

extern void *_end;              /**< linker provides end of image       */
extern void *memheap;           /**< bottom of heap                     */

/* Memory function prototypes */
void *memget(uint);
syscall memfree(void *, uint);
void *stkget(uint);

#endif                          /* _MEMORY_H_ */
