/**
 * @file memset.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <string.h>

/** 
 * @ingroup libxc
 *
 * Fills a region of memory with a byte.
 *
 * @param s
 *      pointer to the memory to place byte into
 * @param c
 *      byte to place
 * @param n
 *      length of region to fill, in bytes
 *
 * @return
 *      @p s
 */
void *memset(void *s, int c, size_t n)
{
    unsigned char *p = s;
    size_t i;

    for (i = 0; i < n; i++)
    {
        p[i] = c;
    }
    return s;
}
