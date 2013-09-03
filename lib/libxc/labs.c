/**
 * @file labs.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <stdlib.h>

/**
 * @ingroup libxc
 *
 * Calculates the absolute value of a long integer.
 *
 * @param j
 *      The long integer to get the absolute value of.
 *
 * @return
 *      The absolute value of @p j.  As a special case, if @p j is @c LONG_MIN,
 *      the return value will be undefined because @c -LONG_MIN cannot be
 *      represented in a @c long.
 */
long labs(long j)
{
    if (j < 0)
    {
        j = -j;
    }

    return j;
}
