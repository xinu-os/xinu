/**
 * @file     xsh_netup.c
 * @provides xsh_netup
 *
 * $Id: xsh_netup.c 2108 2009-10-29 05:07:39Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <device.h>
#include <ether.h>
#include <interrupt.h>
#include <ipv4.h>
#include <shell.h>
#include <string.h>
#include <nvram.h>

static void usage(char *command);

/**
 * Shell command (netup) to start a network interface.
 * @param nargs  number of arguments in args array
 * @param args   array of arguments
 * @return OK for success, SYSERR for syntax error
 */
shellcmd xsh_netup(int nargs, char *args[])
{
    int descrp;
    struct netaddr ip;
    struct netaddr mask;
    struct netaddr gateway;
    char *str_ip, *str_mask, *str_gateway;
    char nvramLookup[DEVMAXNAME + NVRAM_STRMAX];

    /* Enable interrupts */
    enable();

    /* Help */
    if ((2 == nargs) && (0 == strncmp(args[1], "--help", 7)))
    {
        usage(args[0]);
        return SHELL_OK;
    }

    /* Parse device */
    if (nargs > 1)
    {
        descrp = getdev(args[1]);
        if (SYSERR == descrp)
        {
            fprintf(stderr, "%s is not a valid device.\n", args[1]);
            return SHELL_ERROR;
        }
    }
    else
    {
        /* assume first ethernet interface */
        descrp = (ethertab[0].dev)->num;
    }

    if (5 == nargs)
    {
        /* grab string data */
        str_ip = args[2];
        str_mask = args[3];
        str_gateway = args[4];
    }
    else if (nargs < 3)
    {
        /* lookup information in nvram */
        bzero(nvramLookup, DEVMAXNAME + NVRAM_STRMAX);
        sprintf(nvramLookup, "%s_%s", (ethertab[0].dev)->name,
                NET_LAN_IPADDR);
#if NVRAM
        str_ip = nvramGet(nvramLookup);
        if (NULL == str_ip)
        {
            str_ip = nvramGet("lan_ipaddr");
        }
#else
        str_ip = NULL;
#endif                          /* NVRAM */

        bzero(nvramLookup, DEVMAXNAME + NVRAM_STRMAX);
        sprintf(nvramLookup, "%s_%s", (ethertab[0].dev)->name,
                NET_SUBNET_MASK);
#if NVRAM
        str_mask = nvramGet(nvramLookup);
        if (NULL == str_mask)
        {
            str_mask = nvramGet("lan_netmask");
        }
#else
        str_mask = NULL;
#endif                          /* NVRAM */

#if NVRAM
        str_gateway = nvramGet(NET_GATEWAY);
        if (NULL == str_gateway)
        {
            str_gateway = nvramGet("lan_gateway");
        }
#else
        str_gateway = NULL;
#endif                          /* NVRAM */
    }
    else
    {
        fprintf(stderr, "Invalid number of arguments\n");
        return SHELL_ERROR;
    }

    /* Make sure we have valid IPv4 style parameters */
    if (NULL == str_ip)
    {
        str_ip = "0.0.0.0";
    }

    if (NULL == str_mask)
    {
        str_mask = "0.0.0.0";
    }

    if (NULL == str_gateway)
    {
        str_gateway = "0.0.0.0";
    }

    /* Parse IP */
    if (SYSERR == dot2ipv4(str_ip, &ip))
    {
        fprintf(stderr, "%s is not a valid IPv4 address.\n", str_ip);
        return SHELL_ERROR;
    }

    /* Parse Mask */
    if (SYSERR == dot2ipv4(str_mask, &mask))
    {
        fprintf(stderr, "%s is not a valid IPv4 address mask.\n",
                str_mask);
        return SHELL_ERROR;
    }

    /* Parse Gateway */
    if (SYSERR == dot2ipv4(str_gateway, &gateway))
    {
        fprintf(stderr, "%s is not a valid IPv4 address.\n", str_gateway);
        return SHELL_ERROR;
    }

    if (SYSERR == netUp(descrp, &ip, &mask, &gateway))
    {
        fprintf(stderr, "Failed to start network interface on %s\n",
                args[1]);
        return SHELL_ERROR;
    }

    printf("%s is %s with netmask %s (gateway: %s)\n",
           devtab[descrp].name, str_ip, str_mask, str_gateway);

    return SHELL_OK;
}

static void usage(char *command)
{
    printf("Usage:\n");
    printf("\t%s [<DEVICE> [<IP> <MASK> <GATEWAY>]]\n", command);
    printf("Description:\n");
    printf("\tStarts a network interface on the specified underlying\n");
    printf("\tdevice.\n");
    printf("Options:\n");
    printf("\t<DEVICE>\tunderlying device (default: ETH0)\n");
    printf("\t<IP>\t\tIP address\n");
    printf("\t<MASK>\t\tsubnet mask\n");
    printf("\t<GATEWAY>\trouting gateway\n");
    printf("\t--help\t\tdisplay this help and exit\n");
    printf
        ("\tIP, MASK, and GATEWAY will use nvram values if not present.\n");
}
