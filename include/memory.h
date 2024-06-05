/**
 * @file memory.h
 * Definitions for kernel memory allocator and maintenance.
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stddef.h>
#include <stdint.h>

#define MEMORY_DMBDEC (2*sizeof(uintptr_t)-1)
/* roundmb - round address up to size of memblock  */
#define roundmb(x)  (void *)( (MEMORY_DMBDEC + (ulong)(x)) & ~MEMORY_DMBDEC )
/* truncmb - truncate address down to size of memblock */
#define truncmb(x)  (void *)( ((ulong)(x)) & ~MEMORY_DMBDEC )

/**
 * @ingroup memory_mgmt
 *
 * Frees memory allocated with stkget().
 *
 * @param p
 *      Pointer to the topmost (highest address) word of the allocated stack (as
 *      returned by stkget()).
 * @param len
 *      Size of the allocated stack, in bytes.  (Same value passed to stkget().)
 */
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
    uintptr_t length;                    /**< size of memory block (with struct) */
};

extern struct memblock memlist;     /**< head of free memory list           */

/* Other memory data */

extern void *_end;              /**< linker provides end of image           */
extern void *_etext;            /**< linker provides end of text segment    */
extern void *memheap;           /**< bottom of heap                         */

/* Memory function prototypes */
void *memget(uintptr_t);
syscall memfree(void *, uintptr_t);
void *stkget(uintptr_t);

#endif                          /* _MEMORY_H_ */
