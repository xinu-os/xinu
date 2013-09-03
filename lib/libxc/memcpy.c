/**
 * @file memcpy.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <string.h>

/**
 * @ingroup libxc
 *
 * Copy the specified number of bytes of memory to another location.  The memory
 * locations must not overlap.
 *
 * @param dest
 *      Pointer to the destination memory.
 * @param src
 *      Pointer to the source memory.
 * @param n
 *      The amount of data (in bytes) to copy.
 *
 * @return
 *      @p dest
 */
void *memcpy(void *dest, const void *src, size_t n)
{
    unsigned char *dest_p = dest;
    const unsigned char *src_p = src;
    size_t i;

    for (i = 0; i < n; i++)
    {
        dest_p[i] = src_p[i];
    }

    return dest;
}
