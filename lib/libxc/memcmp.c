/**
 * @file memcmp.c
 */

/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <string.h>

/**
 * @ingroup libxc
 *
 * Compares two memory regions of a specified length.
 *
 * @param s1
 *      Pointer to the first memory location.
 * @param s2
 *      Pointer to the second memory location.
 * @param n
 *      Length, in bytes, to compare.
 *
 * @return
 *      A negative value, 0, or a positive value if the @p s1 region of memory
 *      is less than, equal to, or greater than the @p s2 region of memory,
 *      respectively.
 */
int memcmp(const void *s1, const void *s2, size_t n)
{
    const unsigned char *p1 = s1, *p2 = s2;
    size_t i;

    for (i = 0; i < n; i++)
    {
        if (p1[i] != p2[i])
        {
            return (int)p1[i] - (int)p2[i];
        }
    }
    return 0;
}
