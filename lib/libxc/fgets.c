/**
 * @file fgets.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <stdio.h>

/**
 * @ingroup libxc
 *
 * Reads a newline-terminated string from a device.
 *
 * @param s
 *      The buffer in which to place the resulting data.
 * @param n
 *      One less than the maximum number of characters to read.
 * @param dev
 *      The index of the device from which to read.
 *
 * @return
 *      On success, returns a pointer to the buffer @p s.  On @c EOF before any
 *      characters were read, read error, or invalid device, returns @c NULL.
 *      On success, the resulting buffer will contain a null-terminated string
 *      that is the read data up until the first of the next newline, the end of
 *      the buffer, or the point at which @c EOF was reached on the device.
 */
char *fgets(char *s, int n, int dev)
{
    int c = EOF;
    char *cs;

    cs = s;

    /* Read characters until maximum read length,
     * end of line, or end of file. */
    while ((--n > 0) && ((c = fgetc(dev)) != EOF))
    {
        *cs++ = c;
        if ('\n' == c)
        {
            break;
        }
    }

    /* EOF or read error.  TODO: There is currently no way to distinguish read
     * error from EOF, so we treat them the same and only return NULL if no
     * characters were successfully read.  This results in incorrectly returning
     * a pointer to the buffer rather than NULL if a read error occurs after one
     * or more characters were successfully read.  */
    if (c == EOF && cs == s)
    {
        return NULL;
    }

    /* Null-terminate the buffer and return a pointer to it.  */
    *cs = '\0';
    return s;
}
