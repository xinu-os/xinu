/**
 * @file ttyGetc.c
 * @provides ttyGetc.
 *
 * $Id: ttyGetc.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <ctype.h>
#include <device.h>
#include <tty.h>

/**
 * Read a single character from a TTY.
 * @param devptr TTY device table entry
 * @return character read from TTY, or EOF
 */
devcall ttyGetc(device *devptr)
{
    char ch;
    int result = NULL;

    result = ttyRead(devptr, &ch, 1);

    if (1 != result)
    {
        return result;
    }

    return ch;
}
