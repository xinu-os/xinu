/**
 * @file vlanStat.c
 * @provides vlanStat.
 *
 * $Id: vlanStat.c 2106 2009-10-28 21:51:16Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include "bcm4713.h"
#include "bcmswitch.h"
#include <stdio.h>
#include <string.h>

int vlanStat(void)
{
    struct mdioregs *mdio = NULL;
    ulong result = 0;
    int table = 0, port = 0;
    ushort deftags[SWT_PORTS];

    if (NULL == swtregs.mdio)
    {
        fprintf(stderr, "Switch Core not found!\n");
        return 1;
    }
    mdio = swtregs.mdio;

    printf("VLAN Switch Status:\n");

    switchReadReg(mdio, SWT_PAGE_VLAN, SWT_REG_VLAN_CONTROL0, &result);
    printf("\tControl %d : %02X, ", SWT_REG_VLAN_CONTROL0, result);
    switchReadReg(mdio, SWT_PAGE_VLAN, SWT_REG_VLAN_CONTROL1, &result);
    printf("%d : %02X, ", SWT_REG_VLAN_CONTROL1, result);
    switchReadReg(mdio, SWT_PAGE_VLAN, SWT_REG_VLAN_CONTROL2, &result);
    printf("%d : %02X, ", SWT_REG_VLAN_CONTROL2, result);
    switchReadReg(mdio, SWT_PAGE_VLAN, SWT_REG_VLAN_CONTROL3, &result);
    printf("%d : %02X, ", SWT_REG_VLAN_CONTROL3, result);
    switchReadReg(mdio, SWT_PAGE_VLAN, SWT_REG_VLAN_CONTROL4, &result);
    printf("%d : %02X, ", SWT_REG_VLAN_CONTROL4, result);
    switchReadReg(mdio, SWT_PAGE_VLAN, SWT_REG_VLAN_CONTROL5, &result);
    printf("%d : %02X\n", SWT_REG_VLAN_CONTROL5, result);

    printf("\tVLAN Table\n");
    for (table = 0; table < SWT_VLAN_TABLEMAX; table++)
    {
        result = 0x2000 | table;
        switchWriteReg(mdio, SWT_PAGE_VLAN, SWT_REG_VLAN_TABACC, &result);
        switchReadReg(mdio, SWT_PAGE_VLAN, SWT_REG_VLAN_READ, &result);
        if (result & SWT_VLAN_GROUPMASK)
        {
            printf("\t    VID %2d:", table);
            for (port = 0; port < SWT_PORTS; port++)
            {
                if (result & (1 << port))
                {
                    printf(" %d", port);
                    if (!(result & (1 << (port + SWT_PORTS))))
                    {
                        if (port == SWT_CPUPORT)
                        {
                            printf("*");
                        }
                        else
                        {
                            printf("t");
                        }
                    }
                }
            }
            printf("\n");
        }
    }

    for (port = 0; port < SWT_PORTS; port++)
    {
        switchReadReg(mdio,
                      SWT_PAGE_VLAN,
                      SWT_REG_VLAN_TAGPORT0 + 2 * port, &result);
        deftags[port] = result;
        printf("\tDefault Port %d tag: 0x%04X\r\n", port, deftags[port]);
    }

    printf("\n");

    return 0;
}
