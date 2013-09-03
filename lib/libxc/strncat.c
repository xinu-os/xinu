/**
 * @file strncat.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <string.h>

/**
 * @ingroup libxc
 *
 * Concatenate at most the specified number of characters from a possibly
 * null-terminated string to another null-terminated string, always
 * null-terminating the result.  The strings may not overlap.
 *
 * @param dest
 *      Pointer to the null-terminated string to which to concatenate the
 *      additional string or characters.
 * @param src
 *      Pointer to the source string.
 *
 * @param n
 *      Maximum number of bytes of the @p src string to concatenate, excluding
 *      the null terminator that will be appended.
 *
 * @return @p dest
 */
char *strncat(char *dest, const char *src, size_t n)
{
    char *dest_save = dest;
    size_t i;

    dest = strchr(dest, '\0');

    for (i = 0; i < n && src[i] != '\0'; i++)
    {
        dest[i] = src[i];
    }
    dest[i] = '\0';
    return dest_save;
}
