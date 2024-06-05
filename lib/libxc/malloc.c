/**
 * @file malloc.c
 * This file is deprecated in favor of mem/malloc.c and the user heap
 * allocator.  However, it remains here for backup purposes.
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <memory.h>
#include <stdlib.h>

/**
 * @ingroup libxc
 *
 * Request heap storage, record accounting information, returning pointer
 * to assigned memory region.
 *
 * @param size
 *      number of bytes requested
 *
 * @return
 *      pointer to region on success, or @c NULL on failure
 */
void *malloc(size_t size)
{
    struct memblock *pmem;

    /* we don't allocate 0 bytes. */
    if (0 == size)
    {
        return NULL;
    }

    /* make room for accounting info */
    size += sizeof(struct memblock);

    /* acquire memory from kernel */
    pmem = (struct memblock *)memget(size);
    if (SYSERR == (intptr_t)pmem)
    {
        return NULL;
    }

    /* set accounting info */
    pmem->next = pmem;
    pmem->length = size;

    return (void *)(pmem + 1);  /* +1 to skip accounting info */
}
