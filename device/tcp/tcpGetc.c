/**
 * @file tcpGetc.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <device.h>
#include <stddef.h>
#include <tcp.h>

/**
 * @ingroup tcp
 *
 * Read into a single octet from TCP.
 * @param devptr TCP device table entry
 * @return character read from TCP
 */
devcall tcpGetc(device *devptr)
{
    uchar ch;
    int result = 0;

    result = tcpRead(devptr, &ch, 1);

    if (result != 1)
    {
        return result;
    }

    return ch;
}
