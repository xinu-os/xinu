/**
 * @file strcpy.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <string.h>

/**
 * @ingroup libxc
 *
 * Copies a a null-terminated string to the specified location.  The source and
 * destination strings may not overlap.
 *
 * @param dest
 *      Pointer to the memory to which to copy the string.  It must have room
 *      for at least the number of characters in the @p src string, including
 *      the null terminator.
 * @param src
 *      Pointer to the string to copy.
 *
 * @return
 *      A pointer to @p dest.
 */
char *strcpy(char *dest, const char *src)
{
    char *dest_save = dest;

    while ((*dest++ = *src++))
    {
    }
    return dest_save;
}
