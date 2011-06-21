/**
 * @file fprintf.c
 * @provides fprintf.
 *
 * $Id: fprintf.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stdarg.h>

extern void _doprnt(char *, va_list, int (*)(int, int), int);

/**
 * Print a formatted message on specified device (file)
 * @param dev device to write to
 * @param *fmt format string
 * @return 0 if the output was printed sucessfully, -1 if an error occured
 */
int fprintf(int dev, char *fmt, ...)
{
    va_list ap;
    int putc(int, int);

    va_start(ap, fmt);
    _doprnt(fmt, ap, putc, dev);
    va_end(ap);

    return 0;
}
