/**
 * @file httpPutc.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <stddef.h>
#include <http.h>

/**
 * Write a single character to the HTTP device.
 *
 * @param devptr
 *      HTTP device table entry
 * @param ch
 *      character to output
 *
 * @return
 *      @p ch as an <code>unsigned char</code> cast to an @c int on success; @c
 *      SYSERR on failure.
 */
devcall httpPutc(device *devptr, char ch)
{
    struct http *webptr;
    device *phw = NULL;
    int ret;

    /* Setup and error check pointers to structures */
    webptr = &httptab[devptr->minor];
    phw = webptr->phw;
    if (NULL == phw)
    {
        return SYSERR;
    }

    /* Write character to httpWrite */
    ret = httpWrite(devptr, &ch, 1);
    if (ret == 1)
    {
        return (uchar)ch;
    }
    else
    {
        return SYSERR;
    }
}
