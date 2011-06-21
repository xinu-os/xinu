/**
 * @file httpClose.c
 * @provides httpClose.
 *
 * $Id: httpClose.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdlib.h>

#include <http.h>
#include <interrupt.h>
#include <semaphore.h>

/**
 * Close a HTTP device.
 * @param devptr HTTP device table entry
 * @return OK if HTTP is closed properly, otherwise SYSERR
 */
devcall httpClose(device *devptr)
{
    irqmask im;
    struct http *webptr;

    /* Verify that device is open */
    im = disable();
    webptr = &httptab[devptr->minor];
    if (HTTP_STATE_ALLOC != webptr->state)
    {
        restore(im);
        return SYSERR;
    }

    httpFree(devptr);
    semfree(webptr->closeall);

    /* Free memory associated with device malloc calls if necessary */
    if (NULL != webptr->content)
    {
        free(webptr->content);
    }
    if (NULL != webptr->boundary)
    {
        free(webptr->boundary);
    }
    if (NULL != webptr->hostname_str)
    {
        free(webptr->hostname_str);
    }
    if (NULL != webptr->domainname_str)
    {
        free(webptr->domainname_str);
    }
    if (NULL != webptr->lan_ip_str)
    {
        free(webptr->lan_ip_str);
    }
    if (NULL != webptr->subnet_mask_str)
    {
        free(webptr->subnet_mask_str);
    }
    if (NULL != webptr->gate_ip_str)
    {
        free(webptr->gate_ip_str);
    }

    bzero(webptr, sizeof(struct http)); /* Clear HTTP structure.    */
    webptr->state = HTTP_STATE_FREE;
    restore(im);
    return OK;
}
