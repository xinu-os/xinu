/**
 * @file strncpy.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <string.h>

/**
 * @ingroup libxc
 *
 * Copies at most @p n bytes of a string to a new location.  The source string
 * may or may not be null-terminated, but if it is and the null terminator is
 * present in the first @p n bytes, then copying will stop early when it is
 * reached.  If less than @p n bytes were copied, then the remaining bytes in
 * the destination will be set to null characters until @p n bytes total have
 * been written.
 *
 * <b>Beware: if the source string does not contain a null terminator in its
 * first @p n bytes, the destination will not be null-terminated.  To correctly
 * write a null-terminated string into a fixed-size buffer with truncation as
 * expected, consider using strlcpy() instead.</b>
 *
 * @param dest
 *      Pointer to the memory to which to copy the string.
 * @param src
 *      Pointer to the string to copy.
 * @param n
 *      Number of bytes to fill (see above description).
 *
 * @return
 *      A pointer to @p dest.
 */
char *strncpy(char *dest, const char *src, size_t n)
{
    size_t i;
    char *dest_save = dest;

    for (i = 0; src[i] != '\0' && i < n; i++)
    {
        dest[i] = src[i];
    }

    for (; i < n; i++)
    {
        dest[i] = '\0';
    }

    return dest_save;
}
