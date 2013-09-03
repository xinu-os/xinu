/**
 * @file fputc.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <stdio.h>
#include <device.h>

/**
 * @ingroup libxc
 *
 * Writes one character to a device.
 *
 * @param c
 *      The character to write.
 * @param dev
 *      Index of the device to which to write the character.
 *
 * @return
 *      On success, returns the character written as an <code>unsigned
 *      char</code> cast to an @c int.  On write error or invalid device,
 *      returns @c EOF.
 */
int fputc(int c, int dev)
{
    int ret;

    ret = putc(dev, c);
    if (ret == SYSERR || ret == EOF)
    {
        return EOF;
    }
    else
    {
        return (int)(unsigned char)ret;
    }
}
