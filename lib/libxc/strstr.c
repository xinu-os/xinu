/**
 * @file strstr.c
 */

/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <string.h>
#include <stddef.h>

/** 
 * @ingroup libxc
 *
 * Returns a pointer to the first location in a null-terminated string at which
 * a particular substring appears.
 *
 * @param haystack
 *      The string to search.
 * @param needle
 *      The string to locate.
 *
 * @return
 *      The pointer in the string, or @c NULL if the string was not found.
 */
char *strstr(const char *haystack, const char *needle)
{
    size_t needle_len = strlen(needle);
    for (; *haystack != '\0'; haystack++)
    {
        if (strncmp(haystack, needle, needle_len) == 0)
        {
            return (char*)haystack; /* Cast away const. */
        }
    }
    return NULL;
}
