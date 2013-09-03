/**
 * @file atoi.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <stdlib.h>

/**
 * @ingroup libxc
 *
 * Converts the initial portion of an ASCII null-terminated string into an
 * integer.  Leading whitespace is skipped and an optional <code>+</code> or
 * <code>-</code> character is allowed to indicate sign.  Parsing otherwise
 * stops once the first nondigit character is encountered.  If no digits have
 * been encountered at that point, 0 shall be returned.
 *
 * @param nptr
 *      Pointer to the string to convert.
 *
 * @return
 *      The resulting integer.
 */
int atoi(const char *nptr)
{
    return atol(nptr);
}
