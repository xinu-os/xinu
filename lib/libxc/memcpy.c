/**
 * @file memcpy.c
 * @provides memcpy.
 *
 * $Id: memcpy.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

/**
 * Memory copy, copy a location in memory from src to dst.
 * @param s destination location
 * @param ct source location
 * @param n amount of data (in bytes) to copy
 * @return pointer to destination
 */
void *memcpy(void *s, const void *ct, int n)
{
    register int i;
    char *dst = (char *)s;
    char *src = (char *)ct;

    for (i = 0; i < n; i++)
    {
        *dst++ = *src++;
    }
    return s;
}
