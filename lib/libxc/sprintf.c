/**
 * @file sprintf.c
 * @provides sprintf.
 *
 * $Id: sprintf.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stdarg.h>

static int sprntf(int, int);
extern void _doprnt(char *, va_list, int (*func) (int, int), int);

/**
 * Format arguments and place output in a string
 * @param *str output string
 * @param *fmt format string
 */
int sprintf(char *str, char *fmt, ...)
{
    va_list ap;
    char *s;

    s = str;
    va_start(ap, fmt);
    _doprnt(fmt, ap, sprntf, (int)&s);
    va_end(ap);
    *s++ = '\0';

    return ((int)str);
}

/**
 * Routine called by _doprnt to handle each character
 */
static int sprntf(int acpp, int ac)
{
    char **cpp = (char **)acpp;
    char c = (char)ac;

    return (*(*cpp)++ = c);
}
