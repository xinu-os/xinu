/**
 * @file httpValidations.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>

#include <http.h>
#include <string.h>

/**
 * Determines if the passed HTTP method is allowed.
 * @param method string associated with the HTTP method to check
 * @param mlen length of method string to check
 * @return method constant if valid method, otherwise SYSERR
 */
int validMethod(char *method, int mlen)
{
    int retval;

    /* Allowed request methods */
    char *GET = "GET";
    char *POST = "POST";
    char *HEAD = "HEAD";

    /* Not allowed methods */
    char *PUT = "PUT";
    char *DELETE = "DELETE";
    char *TRACE = "TRACE";
    char *CONNECT = "CONNECT";


    if ((mlen == strnlen(GET, HTTP_STR_SM)) &&
        (0 == memcmp(method, GET, mlen)))
    {
        retval = HTTP_METHOD_GET;
    }
    else if ((mlen == strnlen(POST, HTTP_STR_SM)) &&
             (0 == memcmp(method, POST, mlen)))
    {
        retval = HTTP_METHOD_POST;
    }
    else if ((mlen == strnlen(HEAD, HTTP_STR_SM)) &&
             (0 == memcmp(method, HEAD, mlen)))
    {
        retval = HTTP_METHOD_HEAD;
    }

    else if (((mlen == strnlen(POST, HTTP_STR_SM)) &&
              (0 == memcmp(method, PUT, mlen))) ||
             ((mlen == strnlen(DELETE, HTTP_STR_SM)) &&
              (0 == memcmp(method, DELETE, mlen))) ||
             ((mlen == strnlen(TRACE, HTTP_STR_SM)) &&
              (0 == memcmp(method, TRACE, mlen))) ||
             ((mlen == strnlen(CONNECT, HTTP_STR_SM)) &&
              (0 == memcmp(method, CONNECT, mlen))))
    {
        retval = HTTP_METHOD_NOALLOW;
    }
    else
    {
        retval = SYSERR;
    }

    return retval;
}

/**
 * Determines if the passed HTTP version is allowed.
 * @param version string associated with the HTTP version to check
 * @param vlen length of the version string to check
 * @return version constant if valid method, otherwise SYSERR
 */
int validVersion(char *version, int vlen)
{
    int retval;

    /* Allowed request methods */
    char *HTTP11 = "HTTP/1.1";
    char *HTTP10 = "HTTP/1.0";

    if ((vlen == strnlen(HTTP11, HTTP_STR_SM)) &&
        (0 == memcmp(version, HTTP11, vlen)))
    {
        retval = HTTP_VERSION_11;
    }
    else if ((vlen == strnlen(HTTP10, HTTP_STR_SM))
             && (0 == memcmp(version, HTTP10, vlen)))
    {
        retval = HTTP_VERSION_10;
    }
    else
    {
        retval = SYSERR;
    }

    return retval;
}

/**
 * Determines if the passed URI is a valid request.
 * @param url string associated with the url to check
 * @param ulen length of the url string to check
 * @return index into httpcmdtab of page to process, otherwise SYSERR
 */
int validURI(char *url, int ulen)
{
    int i;

    /* Check for index request without a url (just a forward slash) */
    if ((1 == ulen) && (0 == memcmp("/", url, ulen)))
    {
        return 0;
    }

    /* Move past the forward slash and adjust length */
    url++;
    ulen--;

    /* Check all the commands in the web shell array set */
    for (i = 0; i < nhttpcmd; i++)
    {
        if ((ulen == strnlen(httpcmdtab[i].url, HTTP_STR_SM)) &&
            (0 == memcmp(httpcmdtab[i].url, url, ulen)))
        {
            return i;
        }
    }

    /* Could not find the url */
    return SYSERR;
}
