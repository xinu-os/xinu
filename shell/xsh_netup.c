/**
 * @file     xsh_netup.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <conf.h>

#if NETHER

#include <device.h>
#include <ether.h> /* For ethertab */
#include <ipv4.h>
#include <shell.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef WITH_DHCPC
#  include <dhcpc.h>
#endif

#if NVRAM
#  include <nvram.h>
#endif

static void usage(const char *command);

/**
 * @ingroup shell
 *
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
    struct netaddr *gatewayptr;
    const char *str_ip, *str_mask, *str_gateway;
#if NVRAM
    char nvramLookup[DEVMAXNAME + NVRAM_STRMAX];
#endif
    const char *devname;
    int result;

    /* Help */
    if ((2 == nargs) && (0 == strcmp(args[1], "--help")))
    {
        usage(args[0]);
        return SHELL_OK;
    }

    /* Parse device if specifed */
    if (nargs > 1)
    {
        descrp = getdev(args[1]);
        devname = args[1];
        if (SYSERR == descrp)
        {
            fprintf(stderr, "ERROR: %s is not a valid device.\n", devname);
            return SHELL_ERROR;
        }
    }
    else
    {
        /* assume first ethernet interface */
        descrp = (ethertab[0].dev)->num;
        devname = (ethertab[0].dev)->name;
        fprintf(stderr, "WARNING: defaulting to network device %s\n",
                devname);
    }

    str_ip = NULL;
    str_mask = NULL;
    str_gateway = NULL;

    /* IP address specified?  */
    if (nargs >= 3)
    {
        str_ip = args[2];

        /* Mask specified?  */
        if (nargs >= 4)
        {
            str_mask = args[3];

            /* Gateway specified?  */
            if (nargs >= 5)
            {
                str_gateway = args[4];
            }
        }
    }

    /* lookup missing information in nvram */
#if NVRAM
    if (NULL == str_ip)
    {
        sprintf(nvramLookup, "%s_%s", devname, NET_LAN_IPADDR);
        str_ip = nvramGet(nvramLookup);
        if (NULL == str_ip)
        {
            str_ip = nvramGet("lan_ipaddr");
        }
    }

    if (NULL == str_mask)
    {
        sprintf(nvramLookup, "%s_%s", devname, NET_SUBNET_MASK);
        str_mask = nvramGet(nvramLookup);
        if (NULL == str_mask)
        {
            str_mask = nvramGet("lan_netmask");
        }
    }

    if (NULL == str_gateway)
    {
        str_gateway = nvramGet(NET_GATEWAY);
        if (NULL == str_gateway)
        {
            str_gateway = nvramGet("lan_gateway");
        }
    }
#endif  /* NVRAM */

    if (NULL != netLookup(descrp))
    {
        fprintf(stderr, "ERROR: %s is already up.  Please use 'netdown' "
                "to bring it down first.\n", devname);
        return SHELL_ERROR;
    }

    if (descrp < ETH0 || descrp >= ETH0 + NETHER)
    {
        fprintf(stderr, "ERROR: %s is not a network device.\n", devname);
        return SHELL_ERROR;
    }

    if (NULL != str_ip)
    {
        /* Static configuration  */

        /* Use default mask if it was not specified--- this happens if it was
         * not specified on the command line and was not found in the nvram (if
         * nvram exists on this platform).  */
        if (NULL == str_mask)
        {
            str_mask = "255.255.255.0";
            fprintf(stderr, "WARNING: defaulting to mask %s\n", str_mask);
        }

        /* Parse IP */
        if (SYSERR == dot2ipv4(str_ip, &ip))
        {
            fprintf(stderr, "ERROR: %s is not a valid IPv4 address.\n", str_ip);
            return SHELL_ERROR;
        }

        /* Parse Mask */
        if (SYSERR == dot2ipv4(str_mask, &mask))
        {
            fprintf(stderr, "ERROR: %s is not a valid IPv4 address mask.\n",
                    str_mask);
            return SHELL_ERROR;
        }

        /* Parse gateway if specified.  Note: a gateway is not required so it
         * has no default value; we only warn that no gateway is being set.  */
        if (NULL != str_gateway)
        {
            if (SYSERR == dot2ipv4(str_gateway, &gateway))
            {
                fprintf(stderr, "ERROR: %s is not a valid "
                        "IPv4 address.\n", str_gateway);
                return SHELL_ERROR;
            }
            gatewayptr = &gateway;
        }
        else
        {
            fprintf(stderr, "WARNING: defaulting to no gateway\n");
            gatewayptr = NULL;
        }
    }
    else
    {
        /* IP address not specified; use DHCP if available.  */
    #ifdef WITH_DHCPC
        struct dhcpData data;

        printf("Trying DHCP on %s...\n", devname);

        /* Wait at most 10 seconds before timing out.  */
        result = dhcpClient(descrp, 10, &data);

        if (TIMEOUT == result)
        {
            fprintf(stderr, "ERROR: DHCP autoconfiguration "
                    "on %s timed out\n", devname);
            return SHELL_ERROR;
        }
        else if (OK != result)
        {
            fprintf(stderr, "ERROR: DHCP autoconfiguration "
                    "on %s failed\n", devname);
            return SHELL_ERROR;
        }
        netaddrcpy(&ip, &data.ip);
        netaddrcpy(&mask, &data.mask);
        if (0 != data.gateway.len)
        {
            /* Gateway was provided.  */
            netaddrcpy(&gateway, &data.gateway);
            gatewayptr = &gateway;
        }
        else
        {
            /* No gateway provided.  */
            gatewayptr = NULL;
        }
    #else
        fprintf(stderr,
            "ERROR: DHCP not supported!  Either recompile Embedded Xinu with\n"
            "       WITH_DHCPC defined in xinu.conf, or specify the IP address\n"
            "       manually (try `? %s')\n", args[0]);
        return SHELL_ERROR;
    #endif
    }

    result = netUp(descrp, &ip, &mask, gatewayptr);

    if (OK == result)
    {
        char str_ip[20];
        char str_mask[20];
        char str_gateway[20];

        netaddrsprintf(str_ip, &ip);
        netaddrsprintf(str_mask, &mask);
        if (NULL != gatewayptr)
        {
            netaddrsprintf(str_gateway, gatewayptr);
            printf("%s is %s with netmask %s (gateway %s)\n",
                   devname, str_ip, str_mask, str_gateway);
        }
        else
        {
            printf("%s is %s with netmask %s (no gateway)\n",
                   devname, str_ip, str_mask);
        }

        return SHELL_OK;
    }
    else
    {
        fprintf(stderr, "ERROR: Failed to bring up network interface on %s\n",
                devname);
        return SHELL_ERROR;
    }
}

static void usage(const char *command)
{
    printf(
"Usage:\n"
"\t%s [<DEVICE> [<IP> [<MASK> [<GATEWAY>]]]]\n"
"Description:\n"
"\tStarts a network interface on the specified underlying\n"
"\tdevice.\n"
"Options:\n"
"\t<DEVICE>\tunderlying device\n"
"\t<IP>\t\tIP address\n"
"\t<MASK>\t\tsubnet mask\n"
"\t<GATEWAY>\trouting gateway\n"
"\t--help\t\tdisplay this help and exit\n"
"\n"
"\tIf not specified, DEVICE defaults to ETH0.\n"
"\tIf not specified, IP defaults to the nvram value if present,\n"
"\t  otherwise the IP address, mask and gateway will be assigned\n"
"\t  using DHCP if possible.\n"
"\tIf not specified and not using DHCP, MASK defaults to the nvram\n"
"\t  value if present, otherwise 255.255.255.0.\n"
"\tIf not specified and not using DHCP, GATEWAY defaults to the\n"
"\t  nvram value if present, otherwise no gateway.\n"
    , command);
}

#endif /* NETHER */
