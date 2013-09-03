/**
 * @file strcmp.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <string.h>

/**
 * @ingroup libxc
 *
 * Compare two null-terminated strings.
 *
 * @param s1
 *      Pointer to the first string.
 * @param s2
 *      Pointer to the second string.
 *
 * @return
 *      A negative value, 0, or a positive value if the @p s1 string is less
 *      than, equal to, or greater than the @p s2 string, respectively.
 */
int strcmp(const char *s1, const char *s2)
{
    while (*s1 == *s2 && *s1 != '\0')
    {
        s1++;
        s2++;
    }

    return (int)(unsigned char)*s1 - (int)(unsigned char)*s2;
}
