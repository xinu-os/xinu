/**
 * @file strlcpy.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <string.h>

/**
 * @ingroup libxc
 *
 * Copy a null-terminated string into a fixed-size buffer.  Unlike strncpy(),
 * this will perform truncation as expected, and the resulting destination
 * string is guaranteed to be null-terminated (unless @p destsize was 0).
 *
 * @param dest
 *      Pointer to the buffer into which to copy the string.
 * @param src
 *      Pointer to the null-terminated string to copy.
 * @param destsize
 *      Size of the @p dest buffer.
 *
 * @return
 *      Length of the @p src string.  This can be tested to detect whether
 *      truncation was performed.
 */
size_t strlcpy(char *dest, const char *src, size_t destsize)
{
    size_t srclen;
    size_t copylen;

    srclen = strlen(src);
    if (destsize != 0)
    {
        if (destsize - 1 < srclen)
        {
            copylen = destsize - 1;
        }
        else
        {
            copylen = srclen;
        }
        memcpy(dest, src, copylen);
        dest[copylen] = '\0';
    }
    return srclen;
}
