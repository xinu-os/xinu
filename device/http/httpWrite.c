/**
 * @file     httpWrite.c
 * @provides httpWrite
 *
 * $Id: httpWrite.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <http.h>
#include <string.h>

/**
 * Write a buffer to a http
 * @param devptr HTTP device table entry
 * @param buf buffer of characters to output
 * @param len size of the buffer
 * @return count of characters output to underlying device
 */
devcall httpWrite(device *devptr, void *buf, uint len)
{
    struct http *webptr;
    int endlen;

    device *phw = NULL;
    uint count = 0;
    char *buffer = buf;
    char *endpage = "</pre></td></tr></table>"
        "<!end content>" "</BODY></HTML>\0";
    endlen = strnlen(endpage, HTTP_STR_SM);

    /* Setup and error check pointers to structures */
    webptr = &httptab[devptr->minor];
    phw = webptr->phw;
    if (NULL == phw)
    {
        return SYSERR;
    }

    /* Awaiting next request, disregard any shell output */
    if (webptr->flags & HTTP_FLAG_AWAITINGRQST)
    {
        return 0;
    }

    /* Clear buffer, if flag is set */
    if (webptr->flags & HTTP_FLAG_CLEARWOUT)
    {
        bzero(&webptr->wout, HTTP_WBLEN);
        webptr->wcount = 0;
        httpControl(devptr, HTTP_CTRL_SET_FLAG, HTTP_FLAG_CLEANSED, NULL);
        return 0;
    }

    /* Write intermediate buffer if flush flag set or */
    /* passed buffer does not fit in intermediate.    */
    if ((webptr->wcount > 0) &&
        ((webptr->flags & HTTP_FLAG_FLUSHWOUT) ||
         ((webptr->wcount + len) > HTTP_WBLEN)))
    {
        int cursize = 0;

        /* Clear the output buffer */
        bzero(webptr->out, HTTP_OBLEN);

        /* Place chunk information into output buffer if needed */
        if (webptr->flags & HTTP_FLAG_CHUNKED)
        {
            sprintf(webptr->out, "%x\r\n", webptr->wcount);
            cursize = strnlen(webptr->out, HTTP_MAX_CHUNK);
        }

        /* Copy intermediate buffer to output buffer */
        memcpy(&webptr->out[cursize], webptr->wout, webptr->wcount);
        cursize += webptr->wcount;

        /* Clear out intermediate buffer */
        bzero(webptr->wout, webptr->wcount);
        webptr->wcount = 0;

        /* Add final newline if necessary */
        if (webptr->flags & HTTP_FLAG_CHUNKED)
        {
            webptr->out[cursize++] = '\r';
            webptr->out[cursize++] = '\n';
        }

        /* Write buffer to underlying device */
        (*phw->write) (phw, webptr->out, cursize);
        count = len;
    }

    /* If new buffer cannot fit in storage write it out immediately */
    if (len > HTTP_WBLEN)
    {
        bzero(webptr->out, HTTP_OBLEN);
        int cursize = 0;
        if (webptr->flags & HTTP_FLAG_CHUNKED)
        {
            sprintf(webptr->out, "%x\r\n", len);
            cursize = strnlen(webptr->out, HTTP_MAX_CHUNK);

            /* Write chunked information */
            (*phw->write) (phw, webptr->out, cursize);
            bzero(webptr->out, cursize);
        }

        /* Clear prompts and write buffer to underlying device */
        cursize = cleanPrompts(devptr, buffer, len, NULL);
        (*phw->write) (phw, buffer, cursize);

        /* Write terminating '\r\n' if chunked encoding is set */
        if (webptr->flags & HTTP_FLAG_CHUNKED)
        {
            webptr->out[0] = '\r';
            webptr->out[1] = '\n';
            cursize = 2;

            /* Write chunked information */
            (*phw->write) (phw, webptr->out, cursize);
            bzero(webptr->out, cursize);
        }

        count = len;
    }

    if (len > 0 && len <= HTTP_WBLEN)
    {
        /* Place passed buffer into intermediate storage buffer */
        memcpy(&webptr->wout[webptr->wstart + webptr->wcount],
               buffer, len);
        webptr->wcount += len;
        httpControl(devptr, HTTP_CTRL_CLR_FLAG, HTTP_FLAG_CLEANSED, NULL);
        count = len;

        /* Clear out shell prompts from intermediate buffer */
        webptr->wcount =
            cleanPrompts(devptr, webptr->wout, webptr->wcount, webptr);
    }

    /* Send end of page */
    if (!(webptr->flags & HTTP_FLAG_WAITONSHELL) &&
        (webptr->flags & HTTP_FLAG_ENDREADOUTPUT))
    {
        /* Print remains in buffer + chunk info if needed */
        if (webptr->wcount > 0)
        {
            /* Clear the output buffer */
            bzero(webptr->out, HTTP_OBLEN);
            int cursize = 0;

            /* Add chunk information to output buffer if needed */
            if (webptr->flags & HTTP_FLAG_CHUNKED)
            {
                sprintf(webptr->out, "%x\r\n", webptr->wcount);
                cursize = strnlen(webptr->out, HTTP_MAX_CHUNK);
            }

            /* Copy intermediate buffer to output buffer */
            memcpy(&webptr->out[cursize], webptr->wout, webptr->wcount);
            cursize += webptr->wcount;

            /* Clear the intermediate buffer */
            bzero(webptr->wout, webptr->wcount);
            webptr->wcount = 0;

            /* Append final newline to chunk */
            webptr->out[cursize++] = '\r';
            webptr->out[cursize++] = '\n';

            /* Output buffer to underlying device */
            (*phw->write) (phw, webptr->out, cursize);
        }

        /* Print end of page */
        bzero(webptr->out, HTTP_OBLEN);
        if (webptr->flags & HTTP_FLAG_CHUNKED)
        {
            sprintf(webptr->out, "%x\r\n%s\r\n%x\r\n\r\n",
                    endlen + 2, endpage, 0);
        }
        else
        {
            sprintf(webptr->out, "%s\r\n\r\n", endpage);
        }

        (*phw->write) (phw, webptr->out,
                       strnlen(webptr->out, HTTP_OBLEN));

        webptr->wcount = 0;
        httpControl(devptr, HTTP_CTRL_SET_FLAG, HTTP_FLAG_AWAITINGRQST,
                    NULL);
        httpControl(devptr, HTTP_CTRL_SET_FLAG, HTTP_FLAG_CONCLOSE, NULL);

        signal(webptr->closeall);
    }

    return count;
}
