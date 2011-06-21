/**
 * @file abs.c
 * @provides abs.
 *
 * $Id: abs.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

/**
 * Calculates the absolute value of a number.
 * @param arg number to get absolute value of
 * @return absolute value of arg
 */
int abs(int arg)
{
    if (arg < 0)
        arg = -arg;
    return (arg);
}
