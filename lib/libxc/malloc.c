/**
 * @file malloc.c
 * @provides malloc.c.
 * This file is deprecated in favor of mem/malloc.c and the user heap
 * allocator.  However, it remains here for backup purposes.
 *
 * $Id: malloc.c 2065 2009-09-04 21:44:36Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <memory.h>

/**
 * Request heap storage, record accounting information, returning pointer
 * to assigned memory region.
 * @param nbytes number of bytes requested
 * @return pointer to region on success, NULL on failure
 */
__attribute__ ((deprecated))
void *malloc(ulong nbytes)
{
    struct memblock *pmem;

    /* we don't allocate 0 bytes. */
    if (0 == nbytes)
    {
        return NULL;
    }

    /* make room for accounting info */
    nbytes += sizeof(struct memblock);

    /* acquire memory from kernel */
    pmem = (struct memblock *)memget(nbytes);
    if (SYSERR == (uint)pmem)
    {
        return NULL;
    }

    /* set accounting info */
    pmem->next = pmem;
    pmem->length = nbytes;

    return (void *)(pmem + 1);  /* +1 to skip accounting info */
}
