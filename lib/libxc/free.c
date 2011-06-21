/**
 * @file free.c
 * @provides free.
 * This file is deprecated in favor of mem/free.c and the user heap
 * allocator.  However, it remains here for backup purposes.
 *
 * $Id: free.c 2065 2009-09-04 21:44:36Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <memory.h>

/**
 * Attempt to free block of memory based on malloc() accounting
 * information stored in preceding two words.
 * @param *pmem pointer to memory block
 * @return OK on success, SYSERR on failure
 */
__attribute__ ((deprecated))
void free(void *pmem)
{
    struct memblock *block;

    /* block points at the memblock we want to free */
    block = (struct memblock *)pmem;

    /* back up to accounting information */
    block--;

    /* don't memfree if we fail basic checks */
    if (block->next != block)
    {
        return;
    }

    memfree(block, block->length);
}
