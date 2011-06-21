/**
 * @file printf.c
 * @provides printf.
 *
 * $Id: printf.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stdio.h>
#include <stdarg.h>

int printf(const char *fmt, ...)
{
    va_list ap;
    int putc(int, int);

    va_start(ap, fmt);
    _doprnt((char *)fmt, ap, putc, stdout);
    va_end(ap);

    return 0;
}
