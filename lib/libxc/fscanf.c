/**
 * @file fscanf.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <stdarg.h>
#include <stdio.h>

static int getch(int, int);
static int ungetch(int, int);

struct ch_buf
{
    bool ch_avail;
    uchar c;
};

/**
 * @ingroup libxc
 *
 * Scan input from a device according to the specified format string.
 *
 * @param dev
 *      Index of the device from which to scan input.
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
 * @return
 *      The number of items successfully matched and assigned.
 */
int fscanf(int dev, const char *format, ...)
{
    va_list ap;
    struct ch_buf buf;
    int ret;

    buf.ch_avail = FALSE;
    va_start(ap, format);
    ret = _doscan(format, ap, getch, ungetch, dev, (int)&buf);
    va_end(ap);
    return ret;
}

/* Get a character from the device, storing it in the character buffer in case
 * an unget is requested.  */
static int getch(int dev, int _ch_buf)
{
    struct ch_buf *buf = (struct ch_buf *)_ch_buf;
    int c;

    if (buf->ch_avail)
    {
        /* Use character that was put back with ungetch(). */
        buf->ch_avail = FALSE;
        return buf->c;
    }

    c = fgetc(dev);
    if (c != EOF)
    {
        buf->c = c;
    }
    return c;
}

/* Put back a character.  */
static int ungetch(int dev, int _chbuf)
{
    struct ch_buf *buf = (struct ch_buf *)_chbuf;

    buf->ch_avail = TRUE;
    return buf->c;
}
