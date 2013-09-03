/**
 * @file strrchr.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <string.h>
#include <stddef.h>

/**
 * @ingroup libxc
 *
 * Returns a pointer to the last location in a null-terminated string at which a
 * particular character appears.
 *
 * @param s
 *      The string to search.
 * @param c
 *      The character to locate.
 *
 * @return
 *      The pointer in the string, or @c NULL if the character was not found.
 */
char *strrchr(const char *s, int c)
{
    char *r = NULL;
    unsigned char ch = c;

    do
    {
        if (*s == ch)
        {
            r = (char *)s; /* Cast away const. */
        }
    } while (*s++);

    return r;
}
