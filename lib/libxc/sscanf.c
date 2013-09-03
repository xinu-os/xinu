/**
 * @file sscanf.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

static int sgetch(int, int);
static int sungetch(int, int);

/**
 * @ingroup libxc
 *
 * Scan values from a string according to the specified format.
 *
 * @param str
 *      String from which to read input.
 * @param format
 *      Format string.  Not all standard format specifiers are supported by this
 *      implementation.  See _doscan() for a description of supported conversion
 *      specifications.
 * @param ...
 *      Additional arguments that match those specified in the format string.
 *      Generally these need to be a @a pointer to the corresponding argument so
 *      that the value can be set; for example, a <code>\%d</code> conversion
 *      specifier needs to be matched with an <code>int *</code>.
 *
 * @return number of items successfully matched.
 */
int sscanf(const char *str, const char *format, ...)
{
    va_list ap;
    int ret;

    va_start(ap, format);
    ret = _doscan(format, ap, sgetch, sungetch, 0, (int)&str);
    va_end(ap);

    return ret;
}

/* The first argument to the below functions is ignored, as we only need one
 * argument to specify the current position in the string.  */

static int sgetch(int _ignored, int _str_p)
{
    const char **str_p = (const char **)_str_p;
    const char *str = *str_p;
    int c;

    if (*str == '\0')
    {
        c = EOF;
    }
    else
    {
        c = (unsigned char)(*str);
        *str_p = ++str;
    }
    return c;
}

static int sungetch(int _ignored, int _str_p)
{
    const char **str_p = (const char**)_str_p;
    const char *str = *str_p;
    int c;

    str--;
    c = (unsigned char)(*str);
    *str_p = str;
    return c;
}
