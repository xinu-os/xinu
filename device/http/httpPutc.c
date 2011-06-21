/**
 * @file httpPutc.c
 * @provides httpPutc.
 *
 * $Id: httpPutc.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <http.h>

/**
 * Write a single character to a HTTP.
 * @param devptr HTTP device table entry
 * @param ch character to output
 * @return OK if character was written successfully, otherwise SYSERR
 */
devcall httpPutc(device *devptr, char ch)
{
    struct http *webptr;
    device *phw = NULL;

    /* Setup and error check pointers to structures */
    webptr = &httptab[devptr->minor];
    phw = webptr->phw;
    if (NULL == phw)
    {
        return SYSERR;
    }

    /* Write character to httpWrite */
    return httpWrite(devptr, &ch, 1);
}
