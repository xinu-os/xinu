/**
 * @file fputs.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <stdio.h>

/**
 * @ingroup libxc
 *
 * Writes a null-terminated string to a device.
 *
 * @param s
 *      The null terminated string to write.
 * @param dev
 *      The device to write the string to.
 *
 * @return
 *      A non-negative number on success, or @c EOF on error.
 */
int fputs(const char *s, int dev)
{
    for (; *s; s++)
    {
        if (fputc(*s, dev) == EOF)
        {
            return EOF;
        }
    }
    return 0;
}
