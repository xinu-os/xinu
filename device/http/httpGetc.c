/**
 * @file httpGetc.c
 * @provides httpGetc.
 *
 * $Id: httpGetc.c 2077 2009-09-24 23:58:54Z mschul $
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
