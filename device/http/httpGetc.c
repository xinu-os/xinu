/**
 * @file httpGetc.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <http.h>

/**
 * Read a single character from a HTTP.
 * @param devptr HTTP device table entry
 * @return character read from HTTP, or result if invalid read return
 */
devcall httpGetc(device *devptr)
{
    char ch;
    int result = NULL;

    result = httpRead(devptr, &ch, 1);

    if (1 != result)
    {
        return result;
    }

    return ch;
}
