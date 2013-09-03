/**
 * @file etherClose.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <ether.h>

/* Implementation of etherClose() for the smsc9512; see the documentation for
 * this function in ether.h.  */
devcall etherClose(device *devptr)
{
    /* TODO: need to handle canceling all the outstanding USB requests, etc. */
    return SYSERR;
}
