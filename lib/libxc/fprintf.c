/**
 * @file fprintf.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <stdarg.h>
#include <stdio.h>

/**
 * @ingroup libxc
 *
 * Writes a formatted message to the specified device.
 *
 * @param dev
 *      Index of the device to write to.
 * @param format
 *      The format string.  Not all standard conversion specifications are
 *      supported by this implementation.  See _doprnt() for a description of
 *      supported conversion specifications.
 * @param ...
 *      Arguments matching those in the format string.
 *
 * @return
 *      On success, returns the number of characters written.  On write error,
 *      returns a negative value.
 */
int fprintf(int dev, const char *format, ...)
{
    va_list ap;
    int ret;

    va_start(ap, format);
    ret = _doprnt(format, ap,  (int (*)(int,  uintptr_t)) fputc, (uintptr_t) dev);
    va_end(ap);
    return ret;
}
