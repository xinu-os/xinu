/**
 * @file httpReadHdrs.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdlib.h>

#include <http.h>
#include <string.h>


char *allowed_hdr[] = { "Content-Length: ", "Content-Type: " };

/**
 * Decipher the headers of an HTTP request, extracting values as needed.
 * @param webptr pointer to the HTTP device
 * @return OK if headers were processed, otherwise SYSERR
 */
int httpReadHdrs(struct http *webptr)
{
    int cntr, sum;
    int starthdr, endhdr;       /* Location markers for a single header */
    int hdrlen;                 /* Length of an individual header       */
    int valuelen;               /* Length of header value               */
    char *header;               /* Pointer to an individual header      */
    char *value;                /* Pointer to start of header value     */
    char *valueend;             /* Pointer to end of header value       */

    char *boundarykey = "boundary=";    /* String marker for boundary   */


    cntr = 1;
    for (cntr = 1; cntr < webptr->hdrcount; cntr++)
    {
        /* Assign markers for individual header */
        starthdr = webptr->hdrend[cntr - 1] + 2;        /* Starts after a newline */
        endhdr = webptr->hdrend[cntr];
        hdrlen = endhdr - starthdr;

        /* No header found */
        if (hdrlen <= 0)
        {
            continue;
        }

        /* Acquire memory for this individual header */
        header = (char *)malloc(hdrlen + 1);
        if (NULL == (int)header)
        {
            return SYSERR;
        }

        /* Zero out assigned memory and copy in header string */
        bzero(header, hdrlen + 1);
        memcpy(header, &webptr->rin[starthdr], hdrlen);

        /* Compare to each allowed header then decipher the value   */
        /* Content-Length header */
        if (0 == memcmp(header, allowed_hdr[0],
                        strnlen(allowed_hdr[0], HTTP_STR_SM)))
        {
            /* Locate last space in header (just before header value) */
            value = strrchr(header, ' ');

            sum = 0;
            if (value != NULL)
            {
                /* Actual value starts after the space */
                value++;

                while (*value != NULL)
                {
                    sum *= 10;
                    sum += (int)(*value - '0');
                    value++;
                }
            }

            /* Initialize the device variable for content length */
            webptr->contentlen = sum;
        }
        /* Content-Type header */
        else if (0 ==
                 memcmp(header, allowed_hdr[1],
                        strnlen(allowed_hdr[1], HTTP_STR_SM)))
        {
            /* Locate the keyword boundary and its value */
            value = strstr(header, boundarykey);

            /* Verify a boundary value was discovered */
            if (value != NULL)
            {
                value += strnlen(boundarykey, HTTP_STR_MD);

                /* Find the end of the boundary value */
                valueend = strchr(value, ';');
                if (NULL == valueend)
                {
                    /* boundary is last value in list */
                    valuelen = strnlen(value, HTTP_STR_MD);
                }
                else
                {
                    valuelen = (uint)valueend - (uint)value;
                }

                /* Initialize device structure values */
                webptr->boundarylen = valuelen;
                webptr->boundary = (char *)malloc(valuelen);
                if (NULL == webptr->boundary)
                {
                    return SYSERR;
                }
                memcpy(webptr->boundary, value, valuelen);
            }
            else
            {
                webptr->boundary = NULL;
                webptr->boundarylen = 0;
            }
        }

        /* Free mem associated with individual header */
        if (header != NULL)
        {
            free(header);
        }
    }

    return OK;
}
