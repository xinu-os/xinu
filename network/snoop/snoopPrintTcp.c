/**
 * @file snoopPrintTcp.c
 * 
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <network.h>
#include <snoop.h>
#include <stdio.h>
#include <string.h>
#include <tcp.h>

static void snoopPrintTcpPort(ushort port, char *descrp)
{
    switch (net2hs(port))
    {
    case TCP_PORT_HTTP:
        strcpy(descrp, "(HTTP)");
        break;
    case TCP_PORT_TELNET:
        strcpy(descrp, "(Telnet)");
        break;
    default:
        strcpy(descrp, "");
        break;
    }
}

/**
 * @ingroup snoop
 *
 * Print contents of an TCP packet
 * @param tcp TCP packet
 * @return OK if print successful, SYSERR if error occurs
 */
int snoopPrintTcp(struct tcpPkt *tcp, char verbose)
{
    char descrp[40];
    char output[40];

    /* Error check pointer */
    if (NULL == tcp)
    {
        return SYSERR;
    }

    if (verbose >= SNOOP_VERBOSE_ONE)
    {
        printf(" ----- TCP Header -----\n");
        /* Source Port */
        if (verbose >= SNOOP_VERBOSE_TWO)
        {
            snoopPrintTcpPort(net2hs(tcp->srcpt), descrp);
        }
        else
        {
            strcpy(descrp, "");
        }
        sprintf(output, "%d %s", net2hs(tcp->srcpt), descrp);
        printf("  Src Port: %-25s ", output);

        /* Destination Port */
        if (verbose >= SNOOP_VERBOSE_TWO)
        {
            snoopPrintTcpPort(net2hs(tcp->dstpt), descrp);
        }
        else
        {
            strcpy(descrp, "");
        }
        sprintf(output, "%d %s", net2hs(tcp->dstpt), descrp);
        printf("Dst Port: %-25s\n", output);

        /* Sequence number */
        printf("  Seq Num: %-14u ", net2hl(tcp->seqnum));
        /* Acknowledgement number */
        printf("Ack Num: %-14u ", net2hl(tcp->acknum));
        /* Control flags */
        printf("Ctrl: ");
        if (tcp->control & TCP_CTRL_URG)
        {
            printf("U ");
        }
        if (tcp->control & TCP_CTRL_ACK)
        {
            printf("A ");
        }
        if (tcp->control & TCP_CTRL_PSH)
        {
            printf("P ");
        }
        if (tcp->control & TCP_CTRL_RST)
        {
            printf("R ");
        }
        if (tcp->control & TCP_CTRL_SYN)
        {
            printf("S ");
        }
        if (tcp->control & TCP_CTRL_FIN)
        {
            printf("F ");
        }
        printf("\n");

        if (verbose >= SNOOP_VERBOSE_TWO)
        {
            sprintf(output, "%d bytes", offset2octets(tcp->offset));
            printf("  Offset: %-15s ", output);
            sprintf(output, "%d bytes", net2hs(tcp->window));
            printf("Window: %-15s ", output);
            printf("Chksum: 0x%04X\n", net2hs(tcp->chksum));
        }
    }

    return OK;
}
