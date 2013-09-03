/**
 * @file abs.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <stdlib.h>

/**
 * @ingroup libxc
 *
 * Calculates the absolute value of an integer.
 *
 * @param j
 *      The integer to get the absolute value of.
 *
 * @return
 *      The absolute value of @p j.  As a special case, if @p j is @c INT_MIN,
 *      the return value will be undefined because @c -INT_MIN cannot be
 *      represented in an @c int.
 */
int abs(int j)
{
    if (j < 0)
    {
        j = -j;
    }

    return j;
}
