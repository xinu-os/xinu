/**
 * @file httpConfigPage.c
 * @provides postConfig outputConfig.
 *
 * $Id: httpConfigPage.c 2077 2009-09-24 23:58:54Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <ether.h>
#include <ipv4.h>
#include <http.h>
#include <network.h>
#include <nvram.h>
#include <string.h>


/**
 * Interpret ond act on an HTTP POST request of the config.html page.
 * @param webptr pointer to the HTTP device
 */
int postConfig(struct http *webptr)
{
    int cntr, curlen, valuelen;
    bool nvram_altered;
    char *name;
    char *value, *curval;

    /* Set flag to false initially */
    nvram_altered = FALSE;

    /* Check the content of the POST */
    if (*webptr->content != NULL)
    {
        /* Find the name and value for each variable */
        name = NULL;
        value = NULL;
        for (cntr = 0; cntr < webptr->contentlen; cntr++)
        {
            /* Setup name pointer if not set */
            if (NULL == name)
            {
                name = (webptr->content + cntr);
            }

            /* Replace all plus symbols with spaces */
            if ('+' == *(webptr->content + cntr))
            {
                *(webptr->content + cntr) = ' ';
            }
            /* Check for equals, signifies end of name */
            else if ('=' == *(webptr->content + cntr))
            {
                /* Change the char to terminate the string */
                *(webptr->content + cntr) = '\0';

                /* Setup the value pointer */
                value = (webptr->content + (cntr + 1));
            }
            /* Check for ampersand, signifies end of value */
            else if ('&' == *(webptr->content + cntr))
            {
                /* Change the char to terminate the string */
                *(webptr->content + (cntr)) = '\0';

                curval = nvramGet(name);
                if (NULL != curval)
                {
                    curlen = strnlen(curval, NVRAM_STRMAX);
                    valuelen = strnlen(value, NVRAM_STRMAX);
                    if ((curlen != valuelen) ||
                        (0 != memcmp(curval, value, valuelen)))
                    {
                        nvramSet(name, value);
                        nvram_altered = TRUE;
                    }
                }
                else
                {
                    nvramSet(name, value);
                    nvram_altered = TRUE;
                }

                /* Reset pointers */
                curval = NULL;
                name = NULL;
                value = NULL;
            }
        }

        /* Check for last variable found since end of buffer was reached */
        curval = nvramGet(name);
        if (NULL != curval)
        {
            curlen = strnlen(curval, NVRAM_STRMAX);
            valuelen = strnlen(value, NVRAM_STRMAX);
            if ((curlen != valuelen) ||
                (0 != memcmp(curval, value, valuelen)))
            {
                nvramSet(name, value);
                nvram_altered = TRUE;
            }
        }
        else
        {
            nvramSet(name, value);
            nvram_altered = TRUE;
        }
    }

    if (nvram_altered)
    {
        if (SYSERR == nvramCommit())
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }

    return 2;
}

/**
 * Acquire memory for various configuration char pointers in structure.
 * @param webptr pointer to the HTTP device 
 * @return OK if allocation went properly, otherwise SYSERR
 */
int setupConfig(struct http *webptr)
{
    /* Size variables for NVRAM configuration options */
    int hostname_strsz, domainname_strsz, lan_ip_strsz;
    int subnet_mask_strsz, gate_ip_strsz;

    /* Lengths of NVRAM lookup strings + null termination character */
    hostname_strsz = strnlen(NET_HOSTNAME, NVRAM_STRMAX) + 1;
    domainname_strsz = strnlen(NET_DOMAINNAME, NVRAM_STRMAX) + 1;
    lan_ip_strsz = strnlen(NET_LAN_IPADDR, NVRAM_STRMAX) + 1;
    subnet_mask_strsz = strnlen(NET_SUBNET_MASK, NVRAM_STRMAX) + 1;
    gate_ip_strsz = strnlen(NET_GATEWAY, NVRAM_STRMAX) + 1;

    /* Acquire memory for all the NVRAM get strings */
    webptr->hostname_str = (char *)malloc(hostname_strsz + DEVMAXNAME);
    if (NULL == webptr->hostname_str)
    {
        return SYSERR;
    }
    webptr->domainname_str =
        (char *)malloc(domainname_strsz + DEVMAXNAME);
    if (NULL == webptr->domainname_str)
    {
        free(webptr->hostname_str);
        return SYSERR;
    }
    webptr->lan_ip_str = (char *)malloc(lan_ip_strsz + DEVMAXNAME);
    if (NULL == webptr->lan_ip_str)
    {
        free(webptr->hostname_str);
        free(webptr->domainname_str);
        return SYSERR;
    }
    webptr->subnet_mask_str =
        (char *)malloc(subnet_mask_strsz + DEVMAXNAME);
    if (NULL == webptr->subnet_mask_str)
    {
        free(webptr->hostname_str);
        free(webptr->domainname_str);
        free(webptr->lan_ip_str);
        return SYSERR;
    }
    webptr->gate_ip_str = (char *)malloc(gate_ip_strsz + DEVMAXNAME);
    if (NULL == webptr->gate_ip_str)
    {
        free(webptr->hostname_str);
        free(webptr->domainname_str);
        free(webptr->lan_ip_str);
        free(webptr->subnet_mask_str);
        return SYSERR;
    }

    return OK;
}

/**
 * Finalize the configuration page and output it to the HTTP device.
 * @param devptr pointer to the current device for httpWrite calls
 */
int outputConfig(device *devptr, int flashSuccess)
{
    device *ethptr;
    struct http *webptr;
    int cntr;

    /* Format strings for output to underlying device */
    char *oneforall, *oneper, *formsubmit, *flashReport;
    flashReport = NULL;

    /* Pointers for nvramGet() return values */
    char *configDomainname, *configGatewayIP;
    char *configHostname, *configIP, *configSubnetMask;

    /* Size variables for NVRAM configuration options */
    int hostname_strsz, domainname_strsz, lan_ip_strsz;
    int subnet_mask_strsz, gate_ip_strsz;

    /* Lengths of NVRAM lookup strings + null termination character */
    hostname_strsz = strnlen(NET_HOSTNAME, NVRAM_STRMAX) + 1;
    domainname_strsz = strnlen(NET_DOMAINNAME, NVRAM_STRMAX) + 1;
    lan_ip_strsz = strnlen(NET_LAN_IPADDR, NVRAM_STRMAX) + 1;
    subnet_mask_strsz = strnlen(NET_SUBNET_MASK, NVRAM_STRMAX) + 1;
    gate_ip_strsz = strnlen(NET_GATEWAY, NVRAM_STRMAX) + 1;

    /* Setup device pointer */
    webptr = &httptab[devptr->minor];

    /* Setup local pointers to device character pointers */
    char *hostname_str = webptr->hostname_str;
    char *domainname_str = webptr->domainname_str;
    char *lan_ip_str = webptr->lan_ip_str;
    char *subnet_mask_str = webptr->subnet_mask_str;
    char *gate_ip_str = webptr->gate_ip_str;

    /* Zero out the nvram string lookup buffers */
    bzero(hostname_str, hostname_strsz + DEVMAXNAME);
    bzero(domainname_str, domainname_strsz + DEVMAXNAME);
    bzero(lan_ip_str, lan_ip_strsz + DEVMAXNAME);
    bzero(subnet_mask_str, subnet_mask_strsz + DEVMAXNAME);
    bzero(gate_ip_str, gate_ip_strsz + DEVMAXNAME);


    /* Configuration settings, 1 for all devices */
    oneforall =
        "<form method='POST' name='configform'"
        " action='config.html' target='_self'>"
        "<table align=center>"
        "<tr>"
        "<td>Gateway IP Address:</td>"
        "<td>"
        "<input type='text' name='%s'"
        " value='%s' maxlength='%d' width='32' />"
        "</td>" "</tr>" "<tr>" "<td colspan='2'><hr></td>" "</tr>";

    /* Configuration settings, 1 per device */
    oneper =
        "<tr>"
        "<td colspan='2' align='left'>%s:</td>"
        "</tr>"
        "<tr>"
        "<td>Host Name:</td>"
        "<td>"
        "<input type='text' name='%s'"
        " value='%s' maxlength='%d' width='32' />"
        "</td>"
        "<tr>"
        "<tr>"
        "<td>Domain Name:</td>"
        "<td>"
        "<input type='text' name='%s'"
        " value='%s' maxlength='%d' width='32' />"
        "</td>"
        "<tr>"
        "<td>Local IP Address:</td>"
        "<td>"
        "<input type='text' name='%s'"
        " value='%s' maxlength='%d' width='32' />"
        "</td>"
        "</tr>"
        "<td>Subnet Mask:</td>"
        "<td>"
        "<input type='text' name='%s'"
        " value='%s' maxlength='%d' width='32' />"
        "</td>" "</tr>" "<tr>" "<td colspan='2'><hr></td>" "</tr>";

    formsubmit =
        "<tr>"
        "<td align='center' colspan='2'>"
        "<input type='submit' value='Submit Changes' />"
        "</td>" "</table>" "</form>";

    /* Report on the success of updating router configuration in NVRAM */
    switch (flashSuccess)
    {
    case 0:
        flashReport = "<div style='color:#00ff00' align='center'>"
            "Updated router configuration settings, restart required."
            "</div>";
        break;

    case 1:
        flashReport = "<div style='color:#ff0000' align='center'>"
            "Failed to update router configuration settings." "</div>";
        break;
    default:
        flashReport = NULL;
    }
    if (NULL != flashReport)
    {
        httpWrite(devptr, flashReport, strnlen(flashReport, HTTP_STR_SM));
    }

    /* Settings that pertain to all devices */
    if (NULL == (configGatewayIP = nvramGet(NET_GATEWAY)))
    {
        configGatewayIP = "not set";
    }
    printf(oneforall, NET_GATEWAY, configGatewayIP, IPv4_DOTDEC_MAXLEN);

    /* Settings that are per device */
    for (cntr = 0; cntr < NETHER; cntr++)
    {
        ethptr = ethertab[cntr].dev;
        if (NULL == ethptr)
        {
            continue;
        }

        sprintf(hostname_str, "%s_%s", ethptr->name, NET_HOSTNAME);
        sprintf(domainname_str, "%s_%s", ethptr->name, NET_DOMAINNAME);
        sprintf(lan_ip_str, "%s_%s", ethptr->name, NET_LAN_IPADDR);
        sprintf(subnet_mask_str, "%s_%s", ethptr->name, NET_SUBNET_MASK);

        if (NULL == (configHostname = nvramGet(hostname_str)))
        {
            configHostname = "not set";
        }
        if (NULL == (configDomainname = nvramGet(domainname_str)))
        {
            configDomainname = "not set";
        }
        if (NULL == (configIP = nvramGet(lan_ip_str)))
        {
            configIP = "not set";
        }
        if (NULL == (configSubnetMask = nvramGet(subnet_mask_str)))
        {
            configSubnetMask = "not set";
        }

        printf(oneper,          /* Format string for settings per device */
               ethptr->name,    /* Device name */
               hostname_str, configHostname, NET_HOSTNM_MAXLEN,
               domainname_str, configDomainname, NVRAM_STRMAX,
               lan_ip_str, configIP, IPv4_DOTDEC_MAXLEN,
               subnet_mask_str, configSubnetMask, IPv4_DOTDEC_MAXLEN);
    }

    /* Output the bottom of the configuration page body */
    httpWrite(devptr, formsubmit, strnlen(formsubmit, HTTP_STR_MD));


    /* Free memory of configuration page strings */
    if (hostname_str != NULL)
    {
        free(hostname_str);
    }
    if (domainname_str != NULL)
    {
        free(domainname_str);
    }
    if (lan_ip_str != NULL)
    {
        free(lan_ip_str);
    }
    if (subnet_mask_str != NULL)
    {
        free(subnet_mask_str);
    }
    if (gate_ip_str != NULL)
    {
        free(gate_ip_str);
    }

    return OK;
}
