/**
 * @file strnlen.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <string.h>
#include <stddef.h>

/**
 * @ingroup libxc
 *
 * Returns the length of a null-terminated string, but not more than the
 * specified length.
 *
 * @param s
 *      String to calculate the length of.
 *
 * @param maxlen
 *      Maximum length to return.
 *
 * @return
 *      Length of the string up to but not including the null terminator, or @p
 *      maxlen if the string is longer than @p maxlen bytes.
 */
size_t strnlen(const char *s, size_t maxlen)
{
    size_t n = 0;

    while (s[n] != '\0' && n < maxlen)
    {
        n++;
    }
    return n;
}
