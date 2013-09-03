/**
 * @file uartGetc.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <device.h>
#include <stddef.h>
#include <uart.h>

/**
 * @ingroup uartgeneric
 *
 * Read a single character from a UART.
 *
 * @param devptr
 *      Pointer to the device table entry for a UART.
 *
 * @return
 *      On success, returns the character read as an <code>unsigned char</code>
 *      cast to an <code>int</code>.  On read error, invalid device, or end-of
 *      file, returns SYSERR.
 */
devcall uartGetc(device *devptr)
{
    uchar ch;
    int retval;

    retval = uartRead(devptr, &ch, 1);
    if (retval == 1)
    {
        return ch;
    }
    else
    {
        return SYSERR;
    }
}
