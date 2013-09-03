/*
 * @file     xsh_usbinfo.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <conf.h>

#ifdef WITH_USB

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <usb_subsystem.h>

/**
 * @ingroup shell
 *
 * Shell command (usbinfo).
 * @param nargs  number of arguments in args array
 * @param args   array of arguments
 * @return OK for success, SYSERR otherwise
 */
shellcmd xsh_usbinfo(int nargs, char *args[])
{

    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strncmp(args[1], "--help", 7) == 0)
    {
        printf("Usage: %s\n\n", args[0]);
        printf("Description:\n");
        printf("\tDisplays information about the devices attached to the USB bus\n");
        printf("Options:\n");
        printf("\t--help\tdisplay this help and exit\n");
        return OK;
    }

    /* Check for correct number of arguments */
    if (nargs > 1)
    {
        fprintf(stderr, "%s: too many arguments\n", args[0]);
        fprintf(stderr, "Try '%s --help' for more information\n",
                args[0]);
        return SYSERR;
    }

    /* Call into the core USB driver.  */
    return usbinfo();
}
#endif /* WITH_USB */
