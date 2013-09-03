/**
 * @file memchr.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <string.h>
#include <stddef.h>

/** 
 * @ingroup libxc
 *
 * Returns a pointer to the first location in a region of memory at which a
 * particular byte appears.
 *
 * @param s
 *      A pointer to the memory region to search.
 * @param c
 *      The byte to locate.
 * @param n
 *      The maximum number of bytes to search.
 *
 * @return
 *      A pointer to the first occurrence of @p c in the memory region, or @c
 *      NULL if @p c was not found in the memory region.
 */
void *memchr(const void *s, int c, size_t n)
{
    const unsigned char *p = s;
    unsigned char byte = c;
    size_t i;

    for (i = 0; i < n; i++)
    {
        if (p[i] == byte)
        {
            return (void*)&p[i]; /* Cast away const */
        }
    }
    return NULL;
}
