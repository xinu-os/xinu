/**
 * @file fputc.c
 * @provides fputc.
 *
 * $Id: fputc.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

extern int putc(int, char);

#define	SYSERR   (-1)
#define EOF      (-2)

/**
 * Write a character string to a device (file)
 * @param c string to write
 * @param dev device to write to
 * @return character written, EOF if error
 */
int fputc(int c, int dev)
{
    if (SYSERR == (int)putc(dev, c))
    {
        return EOF;
    }
    else
    {
        return c;
    }
}
