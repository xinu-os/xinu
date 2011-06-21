/*
 * @file     rdate.c
 * @provides rdate
 *
 * $Id: rdate.c 2124 2009-11-11 23:41:30Z kthurow $
 */
/* Embedded Xinu, Copyright (C) 2008.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <device.h>
#include <udp.h>
#include <date.h>
#include <stdlib.h>
#include <shell.h>
#include <ether.h>

/**
 * Sends rdate message to destination and retrieves date
 * @param dest the destination of the rdate message
 * @return aquired date for success, SYSERR for syntax error
 */
long getRdate(char *dest)
{
    ushort dev = 0;
    uchar buf[4];
    struct netaddr host;
    struct netaddr *localhost;
    struct netif *interface;


    bzero((uchar *)buf, 4);

    /* Open Device on rdate port */
    if ((ushort)SYSERR == (dev = udpAlloc()))
    {
        return SYSERR;
    }

    if (SYSERR == dot2ipv4(dest, &host))
    {
        return SYSERR;
    }

    /* Look up local ip info */
    interface = netLookup((ethertab[0].dev)->num);

    if (NULL == interface)
    {
        fprintf(stderr, "No network interface found\n");
        return 1;
    }

    localhost = &(interface->ip);

    if (SYSERR ==
        open(dev, localhost, &host, UDP_PORT_RDATE, UDP_PORT_RDATE))
    {
        return SYSERR;
    }

    /* Write Message to destination */
    if (SYSERR == write(dev, buf, 4))
    {
        return SYSERR;
    }

    /* If we read an invalid amount, then rdate failed */
    if (SYSERR == read(dev, buf, 4))
    {
        return SYSERR;
    }

    /* Close device after completion */
    if (SYSERR == (long)close(dev))
    {
        return SYSERR;
    }

    /* convert byes to long */
    return (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
}


/**
 * Gets rdate and sets local time accordingly
 * @param dest the destination of the rdate message
 * @return OK for success, SYSERR for syntax error
 */
syscall rdate(char *dest)
{
    long date;

    if (SYSERR == (date = getRdate(dest)))
    {
        return SYSERR;
    }

    /* Update the date time in the system */
    set_datetime(date);

    return OK;
}
