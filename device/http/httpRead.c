/**
 * @file httpRead.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdlib.h>

#include <http.h>
#include <shell.h>
#include <string.h>

/* TODO: ensure all headers that need to be parsed are */

/**
 * Read characters from a http.
 * @param devptr pointer to http device
 * @param buf buffer for read characters
 * @param len size of the buffer
 * @return number of characters read, EOF if end of file was reached
 */
devcall httpRead(device *devptr, void *buf, uint len)
{
    /* Pointer to HTTP device structure */
    struct http *webptr;

    /* Pointer to physical hardware */
    device *phw;

    /* HTTP request iteration and storage variables */
    char *command;
    int i, start, slen, uriStart, uriEnd, pagebodytype;
    int methodint, cmdindex;
    bool headersOnly;
    methodint = -1;
    pagebodytype = 0;
    headersOnly = FALSE;

    /* Local pointer for passed buffer */
    char *buffer = buf;

    /* Update firmware OS return value */
    int flashSuccess;
    flashSuccess = -1;

    /* Shell interaction variables */
    int commandlen = 0;
    /* Return value for characters read */
    int count = 0;


    /* Setup and error check pointers to structures */
    webptr = &httptab[devptr->minor];
    phw = webptr->phw;
    if (NULL == phw)
    {
        return SYSERR;
    }

    /* Set all flags to default values */
    webptr->rcount = 0;
    httpControl(devptr, HTTP_CTRL_CLR_FLAG, HTTP_FLAG_CONCLOSE, NULL);
    httpControl(devptr, HTTP_CTRL_CLR_FLAG, HTTP_FLAG_RQSTEND, NULL);
    httpControl(devptr, HTTP_CTRL_CLR_FLAG, HTTP_FLAG_CHUNKED, NULL);
    httpControl(devptr, HTTP_CTRL_CLR_FLAG, HTTP_FLAG_WAITONSHELL, NULL);
    httpControl(devptr, HTTP_CTRL_CLR_FLAG, HTTP_FLAG_ENDREADOUTPUT,
                NULL);
    httpControl(devptr, HTTP_CTRL_CLR_FLAG, HTTP_FLAG_CLEANSED, NULL);
    httpControl(devptr, HTTP_CTRL_CLR_FLAG, HTTP_FLAG_CLEARWOUT, NULL);
    httpControl(devptr, HTTP_CTRL_CLR_FLAG, HTTP_FLAG_FLUSHWOUT, NULL);
    httpControl(devptr, HTTP_CTRL_SET_FLAG, HTTP_FLAG_AWAITINGRQST, NULL);

    /* Read in an entire HTTP request */
    webptr->hdrcount = httpReadRqst(devptr);
    if (SYSERR == webptr->hdrcount)
    {
        /* Reading of underly device failed */
        webptr->hdrcount = 0;
        httpControl(devptr, HTTP_CTRL_SET_FLAG, HTTP_FLAG_AWAITINGRQST,
                    NULL);
        httpControl(devptr, HTTP_CTRL_SET_FLAG, HTTP_FLAG_CONCLOSE, NULL);
        signal(webptr->closeall);
        return SYSERR;
    }

    /* Parse the request headers and acquire values */
    if (SYSERR == httpReadHdrs(webptr))
    {
        /* Memory not available for headers of request */
        httpErrorResponse(devptr, HTTP_ERR_INTSERV);
        return 0;
    }

    /* Acquire the remaining webptr->content of the request */
    if (webptr->contentlen > 0)
    {
        webptr->content = (char *)malloc(webptr->contentlen + 1);
        if (NULL == webptr->content)
        {
            /* Memory not available for content size */
            httpErrorResponse(devptr, HTTP_ERR_INTSERV);
            return 0;
        }
        bzero(webptr->content, webptr->contentlen + 1);

        i = 0;
        for (i = 0; i < webptr->contentlen; i++)
        {
            *(webptr->content + i) = (*phw->getc) (phw);
        }
    }
    else
    {
        webptr->content = NULL;
    }

    /* A whole request has been accumulated */
    httpControl(devptr, HTTP_CTRL_CLR_FLAG, HTTP_FLAG_AWAITINGRQST, NULL);

    /* Clear write buffer */
    httpControl(devptr, HTTP_CTRL_SET_FLAG, HTTP_FLAG_CLEARWOUT, NULL);
    httpWrite(devptr, NULL, 0);
    httpControl(devptr, HTTP_CTRL_CLR_FLAG, HTTP_FLAG_CLEARWOUT, NULL);

    /* Decipher if status line is valid request */
    i = 0;

    /* Move to first value in status line */
    for (; webptr->rin[i] == ' ' && i < webptr->hdrend[0]; i++)
    {                           /* Do nothing */
    }


    /* Determine method string length */
    start = i;
    for (; webptr->rin[i] != ' ' && i < webptr->hdrend[0]; i++)
    {                           /* Do nothing */
    }
    slen = i - start;

    /* Check if valid method */
    methodint = validMethod(&webptr->rin[start], slen);

    if (SYSERR == methodint)
    {
        httpErrorResponse(devptr, HTTP_ERR_BADREQ);
        return 0;
    }
    else if (HTTP_METHOD_NOALLOW == methodint)
    {
        httpErrorResponse(devptr, HTTP_ERR_METHOD);
        return 0;
    }
    else if (HTTP_METHOD_POST == methodint && 0 == webptr->contentlen)
    {
        httpErrorResponse(devptr, HTTP_ERR_BADREQ);
        return 0;
    }
    else if (HTTP_METHOD_HEAD == methodint)
    {
        headersOnly = TRUE;
    }

    /* Move to next value in status line */
    for (; webptr->rin[i] == ' ' && i < webptr->hdrend[0]; i++)
    {                           /* Do nothing */
    }


    /* Store the request URI start and end point */
    uriStart = i;
    for (; webptr->rin[i] != ' ' && i < webptr->hdrend[0]; i++)
    {                           /* Do nothing */
    }
    uriEnd = i;


    /* Move to last value in status line */
    for (; webptr->rin[i] == ' ' && i < webptr->hdrend[0]; i++)
    {                           /* Do nothing */
    }


    /* Determine version string length */
    start = i;
    for (; webptr->rin[i] != ' ' && i < webptr->hdrend[0]; i++)
    {                           /* Do nothing */
    }
    slen = i - start;


    /* Check valid version of HTTP */
    if (SYSERR == validVersion(&webptr->rin[start], slen))
    {
        httpErrorResponse(devptr, HTTP_ERR_BADVERS);
        return 0;
    }

    cmdindex = validURI(&webptr->rin[uriStart], uriEnd - uriStart);
    /* Check valid URI */
    if (SYSERR == cmdindex)
    {
        httpErrorResponse(devptr, HTTP_ERR_NOTFND);
        return 0;
    }

    switch (httpcmdtab[cmdindex].inwebshell)
    {
        /* Commands run in webshell */
    case TRUE:
        commandlen = strnlen(httpcmdtab[cmdindex].command, SHELL_BUFLEN);
        command = httpcmdtab[cmdindex].command;

        /* Fill buffer for shell to read from */
        memcpy(buffer, command, commandlen);

        /* Insert newline into buffer */
        buffer[commandlen] = '\r';
        buffer[commandlen + 1] = '\n';

        /* Setup count variable for eventual return */
        count = commandlen + 2;

        /* Set the flag for waiting on webshell */
        httpControl(devptr, HTTP_CTRL_SET_FLAG, HTTP_FLAG_WAITONSHELL,
                    NULL);
        break;

        /* Commands not run in webshell */
    case FALSE:
        /* Handle index page */
        if (0 == (memcmp(httpcmdtab[cmdindex].url, "index.html",
                         strnlen(httpcmdtab[cmdindex].url, HTTP_STR_SM))))
        {
            httpControl(devptr, HTTP_CTRL_CLR_FLAG, HTTP_FLAG_WAITONSHELL,
                        NULL);
        }
        /* Handle configure router page */
        else if (0 == (memcmp(httpcmdtab[cmdindex].url, "config.html",
                              strnlen(httpcmdtab[cmdindex].url,
                                      HTTP_STR_SM))))
        {
            /* Set up variables for all types of requests */
            pagebodytype = CONFIG_PAGE;

            /* Acquire memory for pointers dealing with configuration */
            if (SYSERR == setupConfig(webptr))
            {
                httpErrorResponse(devptr, HTTP_ERR_INTSERV);
                return 0;
            }

            /* Service config.html POST request */
            if (HTTP_METHOD_POST == methodint)
            {
                flashSuccess = postConfig(webptr);
            }
        }
    }

    /* Write headers */
    httpControl(devptr, HTTP_CTRL_CLR_FLAG, HTTP_FLAG_CHUNKED, NULL);
    httpControl(devptr, HTTP_CTRL_SET_FLAG, HTTP_FLAG_CONCLOSE, NULL);
    char *headers =
        "HTTP/1.1 200 OK\r\n"
        "Connection: close\r\n"
        "Content-Type: text/html; charset=ISO-8859-1\r\n"
        "Transfer-Encoding: chunked\r\n\r\n";
    httpWrite(devptr, headers, strnlen(headers, HTTP_STR_SM));

    /* Flush write buffer */
    httpFlushWBuffer(devptr);

    if (!headersOnly)
    {
        /* Set chunked flag */
        httpControl(devptr, HTTP_CTRL_SET_FLAG, HTTP_FLAG_CHUNKED, NULL);

        /* Output the top of a webpage in the web interface */
        httpHtmlBeginPage(devptr);

        /* Output navigation part a webinterface webpage */
        httpHtmlNavigation(devptr);

        /* Write out the HTML for beginning of page content */
        httpHtmlBeginContent(devptr);

        /* Write out the page body if there is one */
        switch (pagebodytype)
        {
        case CONFIG_PAGE:
            outputConfig(devptr, flashSuccess);
            break;
        }
    }


    /* Set the flag showing preliminary writes are all done */
    httpControl(devptr, HTTP_CTRL_SET_FLAG, HTTP_FLAG_ENDREADOUTPUT,
                NULL);

    /* Flush write buffer, transmitting webpage topping parts */
    httpFlushWBuffer(devptr);

    /* Clear write buffer */
    httpControl(devptr, HTTP_CTRL_SET_FLAG, HTTP_FLAG_CLEARWOUT, NULL);
    httpWrite(devptr, NULL, 0);
    httpControl(devptr, HTTP_CTRL_CLR_FLAG, HTTP_FLAG_CLEARWOUT, NULL);

    /* Reset request end flag */
    httpControl(devptr, HTTP_CTRL_CLR_FLAG, HTTP_FLAG_RQSTEND, NULL);


    /* Free buffers used exclusively in the httpRead */
    if (webptr->content != NULL)
    {
        free(webptr->content);
    }
    if (webptr->boundary != NULL)
    {
        free(webptr->boundary);
    }

    return count;
}
