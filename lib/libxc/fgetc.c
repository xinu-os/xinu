/**
 * @file fgetc.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <stdio.h>
#include <device.h>

/**
 * @ingroup libxc
 *
 * Reads a character from a device.
 *
 * @param dev
 *      Index of device from which to read the character.
 *
 * @return
 *      On success, returns the character read as an <code>unsigned char</code>
 *      cast to an @c int.  On read error, invalid device, or end-of file,
 *      returns @c EOF.
 */
int fgetc(int dev)
{
    int c;

    c = getc(dev);

    if (c == SYSERR || c == EOF)
    {
        return EOF;
    }
    else
    {
        return (int)(unsigned char)c;
    }
}
