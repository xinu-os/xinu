/**
 * @file sprintf.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <stdarg.h>
#include <stdio.h>

static int sprntf(int, int);

/**
 * @ingroup libxc
 *
 * Write a formatted message to a null-terminated string.
 *
 * @param str
 *      The output string, which must be large enough to store the formatted
 *      result, including the null terminator.
 * @param format
 *      The format string.  Not all standard conversion specifications are
 *      supported by this implementation.  See _doprnt() for a description of
 *      supported conversion specifications.
 * @param ...
 *      Arguments matching those in the format string.
 *
 * @return
 *      the number of characters written, excluding the null terminator.
 */
int sprintf(char *str, const char *format, ...)
{
    va_list ap;
    char *s;

    s = str;
    va_start(ap, format);
    _doprnt(format, ap, sprntf, (int)&s);
    va_end(ap);
    *s = '\0';

    return s - str;
}

/*
 * Routine called by _doprnt() to output each character.
 */
static int sprntf(int c, int _sptr)
{
    char **sptr = (char **)_sptr;
    char *s = *sptr;

    *s++ = c;
    *sptr = s;
    return (unsigned char)c;
}
