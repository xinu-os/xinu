/**
 * @file httpFlushWBuffer.c
 * @provides httpFlushWBuffer
 *
 * $Id: httpFlushWBuffer.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <http.h>

/**
 * Flush the intermediate write buffer out to underlying device.
 * @param devptr HTTP device that has the buffer to flush
 */
void httpFlushWBuffer(device *devptr)
{
    /* Set the flush flag */
    httpControl(devptr, HTTP_CTRL_SET_FLAG, HTTP_FLAG_FLUSHWOUT, NULL);

    /* Write a zero length buf, write will flush because of set flag */
    httpWrite(devptr, NULL, 0);

    /* Clear the flush flag */
    httpControl(devptr, HTTP_CTRL_CLR_FLAG, HTTP_FLAG_FLUSHWOUT, NULL);
}
