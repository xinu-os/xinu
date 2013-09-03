/**
 * @file strncmp.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <string.h>

/**
 * @ingroup libxc
 *
 * Compare two null-terminated strings, examining at most the specified number
 * of bytes.
 *
 * @param s1
 *      Pointer to the first string.
 * @param s2
 *      Pointer to the second string.
 * @param n
 *      Maximum number of bytes to compare before returning 0.
 *
 * @return
 *      A negative value, 0, or a positive value if the @p s1 string is less
 *      than, equal to, or greater than the @p s2 string, respectively.
 */
int strncmp(const char *s1, const char *s2, size_t n)
{
    size_t i;

    for (i = 0; i < n; i++)
    {
        if (s1[i] == '\0' || s1[i] != s2[i])
        {
            return (int)(unsigned char)s1[i] - (int)(unsigned char)s2[i];
        }
    }
    return 0;
}
