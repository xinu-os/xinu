/**
 * @file httpErrorResponse.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stdio.h>
#include <http.h>
#include <string.h>

/**
 * Formulate and write HTTP error page
 * @param devptr pointer to the underlying device to write to
 * @param errnum the three digit HTTP error code
 * @return 0 if all went OK otherwise SYSERR
 */
int httpErrorResponse(device *devptr, short errNum)
{
    char *htmlError, *errName, *errDescript;
    struct http *webptr;
    short msgSize;

    /* Setup and error check pointers to structures */
    webptr = &httptab[devptr->minor];

    if (HTTP_STATE_ALLOC != webptr->state)
    {
        return SYSERR;
    }

    htmlError =
        "HTTP/1.1 %d %s\r\n"
        "Connection: close\r\n"
        "Content-Type: text/html; charset=ISO-8859-1\r\n"
        "Content-Length: %d\r\n\r\n"
        /* msgSize counts from here to end of page */
        "<html><head>"
        "<title>%d %s</title>"
        "</head><body>"
        "<h1>%s</h1>" "<hr>" "<p>%s</p>" "</body></html>\r\n\r\n";

    msgSize = 80;               /* initial size of response w/o headers */

    /* Set up the error name and error description for each error */
    switch (errNum)
    {
    case HTTP_ERR_BADREQ:
        errName = "Bad Request";
        errDescript = "The client sent an illegal or malformed request.";
        break;

    case HTTP_ERR_AUTHREQ:
        errName = "Unauthorized";
        errDescript = "The request requires user authentication.";
        break;
    case HTTP_ERR_FORBID:
        errName = "Forbidden";
        errDescript = "You do not have adequate permission for the "
            "server to perform the request.";
        break;
    case HTTP_ERR_NOTFND:
        errName = "Not Found";
        errDescript = "The requsted file could not be found.";
        break;
    case HTTP_ERR_METHOD:
        errName = "Method Not Allowed";
        errDescript = "The method sent in the request is not allowed.";
        break;
    case HTTP_ERR_INTSERV:
        errName = "Internal Server Error";
        errDescript = "The server encountered an unexpected condition "
            "(e.g. lack of memory) and could not fulfill the request.";
        break;
    case HTTP_ERR_NOTIMP:
        errName = "Not Implemented";
        errDescript = "The server does not have the functionality "
            "to fulfill the request.";
        break;
    case HTTP_ERR_BADVERS:
        errName = "HTTP Version Not Supported";
        errDescript = "The HTTP version of the request is not "
            "supported by the server.";
        break;
    default:
        /* Unrecognized error number passed to function */
        return SYSERR;
    }

    msgSize += 3;               /* Add chars for error number */
    msgSize += 2 * strnlen(errName, HTTP_STR_SM);
    msgSize += strnlen(errDescript, HTTP_STR_SM);

    /* Write out the HTML error response page */
    printf(htmlError,           /* Error page format string     */
           errNum, errName,     /* Status line header variables */
           msgSize,             /* Content length variable      */
           errNum, errName, errName,    /* HTML page variables */
           errDescript);

    /* Set chunked flag to off */
    httpControl(devptr, HTTP_CTRL_CLR_FLAG, HTTP_FLAG_CHUNKED, NULL);

    /* Flush write buffer */
    httpFlushWBuffer(devptr);

    /* Update connection values in device structure to close */
    httpControl(devptr, HTTP_CTRL_SET_FLAG, HTTP_FLAG_CONCLOSE, NULL);
    signal(webptr->closeall);

    return 0;
}
