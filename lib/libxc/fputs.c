/**
 * @file fputs.c
 * @provides fputs.
 *
 * $Id: fputs.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

extern int putc(int, char);

/**
 * Write a null-terminated string to a device (file)
 * @param dev device to write to
 * @param *s string to write
 * @return result of last putc
 */
int fputs(char *s, int dev)
{
    int r = 0, c;

    while ((c = (*s++)))
    {
        r = putc(dev, c);
    }
    return r;
}
