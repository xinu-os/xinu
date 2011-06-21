/**
 * @file labs.c
 * @provides labs.
 *
 * $Id: labs.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

/**
 * Calculates the absolute value of a number.
 * @param arg number to get absolute value of
 * @return absolute value of arg
 */
long labs(long arg)
{
    if (arg < 0)
        arg = -arg;
    return (arg);
}
