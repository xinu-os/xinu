/**
 * @file strlen.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <string.h>

/**
 * @ingroup libxc
 *
 * Calculates the length of a null-terminated string.
 *
 * @param s
 *      String to calculate the length of.
 *
 * @return
 *      Length of the string in bytes, not including the null terminator.
 */
size_t strlen(const char *s)
{
    size_t n = 0;

    while (s[n] != '\0')
    {
        n++;
    }
    return n;
}
