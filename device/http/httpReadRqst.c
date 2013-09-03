/**
 * @file httpReadRqst.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <http.h>

/**
 * Read an entire HTTP request into the HTTP read input buffer.
 * @param webptr pointer to the HTTP device
 * @return number of headers if request is read in, otherwise SYSERR
 */
int httpReadRqst(device *devptr)
{
    device *phw;
    bool newline;
    int hdrcount;
    char ch;
    struct http *webptr;

    webptr = &httptab[devptr->minor];
    phw = webptr->phw;
    if (NULL == phw)
    {
        return SYSERR;
    }

    /* Initialize other varialbes */
    hdrcount = 0;
    newline = FALSE;
    ch = '\0';

    /* Reads until end of the request or until the buffer is full. */
    while (!(webptr->flags & HTTP_FLAG_RQSTEND) &&
           (webptr->rcount < HTTP_RBLEN) && (hdrcount < HTTP_MAX_HDRS))
    {
        /* Read character */
        ch = (*phw->getc) (phw);
        if (SYSERR == (int)ch)
        {
            return SYSERR;
        }

        if (webptr->rcount >= 1)
        {
            if ('\r' == webptr->rin[webptr->rcount - 1] && '\n' == ch)
            {
                if (newline)
                    httpControl(devptr, HTTP_CTRL_SET_FLAG,
                                HTTP_FLAG_RQSTEND, NULL);
                else
                {
                    /* Newline reached */
                    newline = TRUE;
                    webptr->hdrend[hdrcount] = webptr->rcount - 1;
                    hdrcount++;
                }

            }
            else if (newline && '\r' == ch)
            {
                newline = TRUE;
            }
            else
                newline = FALSE;
        }

        webptr->rin[webptr->rcount] = ch;
        webptr->rcount++;
    }

    return hdrcount;
}
