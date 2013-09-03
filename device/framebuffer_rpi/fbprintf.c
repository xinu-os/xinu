/**
 * @file kprintf.c
 */

/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <stddef.h>
#include <stdarg.h>
#include <device.h>
#include <stdio.h>
#include <framebuffer.h>

/**
 * @ingroup framebuffer
 *
 * perform a synchronous character write
 * @param *devptr pointer to device on which to write character
 * @param c character to write
 * @return c on success, SYSERR on failure
 */
syscall fbputc(uchar c, device *devptr)
{
    fbPutc(devptr, c);
    return (int)c;
}

/**
 * @ingroup framebuffer
 *
 * perform a synchronous kernel read
 * @param *devptr pointer to device on which to write character
 * @return character read on success, SYSERR on failure
 */
syscall fbgetc(device *devptr)
{
    return SYSERR; //not possible.
}

/**
 * @ingroup framebuffer
 *
 * kernel printf: formatted, unbuffered output to framebuffer
 * Akin to "kprintf" for the serial driver.
 * Alternate: fprintf(FRAMEBUF, "string");
 * @param *fmt pointer to string being printed
 * @return OK on success
 */
syscall fbprintf(char *fmt, ...)
{
    va_list ap;

	irqmask mask = disable();

    va_start(ap, fmt);
    _doprnt(fmt, ap, (int (*)(int, int))fbputc, (int)&devtab[FRAMEBUF]);
    va_end(ap);

	restore(mask);
    return OK;
}
