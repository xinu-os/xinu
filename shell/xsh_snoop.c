/**
 * @file     xsh_snoop.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <conf.h>
#include <ipv4.h>
#include <shell.h>
#include <snoop.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if NETHER
static void usage(char *command)
{
    printf("Usage:\n");
    printf("\t%s [--help]\n", command);
    printf("\t%s [-c COUNT] [-i NETIF] [-s CAPLEN]\n", command);
    printf("\t      [-d] [-dd] [-v] [-vv] [-t TYPE]\n");
    printf("\t      [-da ADDR] [-dp PORT] [-sa ADDR] [-sp PORT]\n");
    printf("Description:\n");
    printf
        ("\tSnoop prints out a description and contents of packets on\n");
    printf
        ("\ta network interface. By default it lists all all inbound\n");
    printf
        ("\tand outbound traffic on all active network interfaces.  It\n");
    printf("\tcan also be used to read a PCAP trace file.\n");
    printf("Output Options:\n");
    printf("\t-d\tDump the packet in hex.\n");
    printf("\t-dd\tDump the packet in hex and ASCII.\n");
    printf("\t--help\tDisplay this help and exit.\n");
    printf("\t-v\tPrint details on the network and transport layer\n");
    printf("\t\theader in each packet.\n");
    printf("\t-vv\tPrint details on the link layer header and more\n");
    printf("\t\tdetails on the network and transport layer headers.\n");
    printf("Capture Options:\n");
    printf("\t-c\tExit after capturing COUNT packets.\n");
    printf("\t-i\tCapture only from the network interface NETIF\n");
    printf("\t-s\tCapture only CAPLEN bytes of each packet. Default\n");
    printf("\t\tcaplen is 65535 bytes.\n");
    printf("Filter Options:\n");
    printf
        ("\t-da\tCapture only packets whose destination IPv4 address\n");
    printf("\t\tis ADDR.\n");
    printf
        ("\t-dp\tCapture only packets whose destination port is PORT.\n");
    printf("\t\tThis is ignored if -t UDP or TCP is not specified.\n");
    printf("\t-sa\tCapture only packets whose source IPv4 address\n");
    printf("\t\tis ADDR.\n");
    printf("\t-sp\tCapture only packets whose source port is PORT.\n");
    printf("\t\tThis is ignored if -t UDP or TCP is not specified.\n");
    printf
        ("\t-t\tCapture only packets of type TYPE.  Valid values for\n");
    printf("\t\ttype are: ARP, ICMP, IPv4, TCP, UDP.\n");
}

static void error(char *arg)
{
    fprintf(stderr, "Invalid argument '%s', try snoop --help\n", arg);
}

static thread snoop(struct snoop *cap, uint count, char dump,
                    char verbose)
{
    bool forever = FALSE;
    struct packet *pkt = NULL;
    if (0 == count)
    {
        forever = TRUE;
    }

    while (forever || count > 0)
    {
        pkt = snoopRead(cap);
        if ((SYSERR == (int)pkt) || (NULL == pkt))
        {
            continue;
        }
        cap->nprint++;
        snoopPrint(pkt, dump, verbose);
        netFreebuf(pkt);
        count--;
    }

    return OK;
}

/**
 * @ingroup shell
 *
 * Shell command (snoop).
 * @param nargs  number of arguments in args array
 * @param args   array of arguments
 * @return OK for success, SYSERR for syntax error
 */
shellcmd xsh_snoop(int nargs, char *args[])
{
    int a;
    uint count = 0;
    uint caplen = 65535;
    char dump = SNOOP_DUMP_NONE;
    char verbose = SNOOP_VERBOSE_NONE;
    char *type = NULL;
    char *dstaddr = NULL;
    ushort dstport = 0;
    char *srcaddr = NULL;
    ushort srcport = 0;
    struct snoop cap;
    char devname[DEVMAXNAME];
    tid_typ tid;

    strlcpy(devname, "ALL", DEVMAXNAME);

    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strncmp(args[1], "--help", 7) == 0)
    {
        usage(args[0]);
        return 0;
    }

    /* Parse arguments */
    for (a = 1; a < nargs; a++)
    {
        if (args[a][0] != '-')
        {
            error(args[a]);
            return 1;
        }

        switch (args[a][1])
        {
            /* Capture count */
        case 'c':
            a++;
            if (a >= nargs)
            {
                error(args[a - 1]);
                return 1;
            }
            count = atoi(args[a]);
            break;
            /* Output dump OR Filter dst addr OR Filter dst port */
        case 'd':
            switch (args[a][2])
            {
                /* Output dump hex and char */
            case 'd':
                dump = SNOOP_DUMP_CHAR;
                break;
                /* Filter destination address */
            case 'a':
                a++;
                if (a >= nargs)
                {
                    error(args[a - 1]);
                    return 1;
                }
                dstaddr = args[a];
                break;
                /* Filter destination port */
            case 'p':
                a++;
                if (a >= nargs)
                {
                    error(args[a - 1]);
                    return 1;
                }
                dstport = atoi(args[a]);
                break;
                /* Output dump hex */
            case '\0':
                dump = SNOOP_DUMP_HEX;
                break;
            default:
                error(args[a]);
                return 1;
            }
            break;
            /* Capture interface */
        case 'i':
            a++;
            if (a >= nargs)
            {
                error(args[a - 1]);
                return 1;
            }
            strlcpy(devname, args[a], DEVMAXNAME);
            break;
            /* Capture size OR Filter src addr OR Filter dst addr */
        case 's':
            switch (args[a][2])
            {
                /* Filter source address */
            case 'a':
                a++;
                if (a >= nargs)
                {
                    error(args[a - 1]);
                    return 1;
                }
                srcaddr = args[a];
                break;
                /* Filter source port */
            case 'p':
                a++;
                if (a >= nargs)
                {
                    error(args[a - 1]);
                    return 1;
                }
                srcport = atoi(args[a]);
                break;
                /* Capture size */
            case '\0':
                a++;
                if (a >= nargs)
                {
                    error(args[a - 1]);
                    return 1;
                }
                caplen = atoi(args[a]);
                break;
            default:
                error(args[a]);
                return 1;
            }
            break;
            /* Select type */
        case 't':
            a++;
            if (a >= nargs)
            {
                error(args[a - 1]);
            }
            type = args[a];
            break;
            /* Output verbose */
        case 'v':
            if (args[a][2] == 'v')
            {
                verbose = SNOOP_VERBOSE_TWO;
            }
            else
            {
                verbose = SNOOP_VERBOSE_ONE;
            }
            break;
        default:
            error(args[a]);
            return 1;
        }
    }

    /* Set filter */
    cap.caplen = caplen;
    cap.promisc = FALSE;
    cap.nprint = 0;
    if (NULL == type)
    {
        cap.type = SNOOP_FILTER_ALL;
    }
    else
    {
        if (0 == strncmp(type, "ARP", 4))
        {
            cap.type = SNOOP_FILTER_ARP;
        }
        else if (0 == strncmp(type, "IPv4", 5))
        {
            cap.type = SNOOP_FILTER_IPv4;
        }
        else if (0 == strncmp(type, "UDP", 4))
        {
            cap.type = SNOOP_FILTER_UDP;
        }
        else if (0 == strncmp(type, "TCP", 4))
        {
            cap.type = SNOOP_FILTER_TCP;
        }
        else if (0 == strncmp(type, "ICMP", 5))
        {
            cap.type = SNOOP_FILTER_ICMP;
        }
        else
        {
            fprintf(stderr, "Invalid type '%s', try usage --help\n",
                    type);
            return 1;
        }
    }
    if (NULL == srcaddr)
    {
        cap.srcaddr.type = NULL;
    }
    else
    {
        dot2ipv4(srcaddr, &cap.srcaddr);
    }
    cap.srcport = srcport;
    if (NULL == dstaddr)
    {
        cap.dstaddr.type = NULL;
    }
    else
    {
        dot2ipv4(dstaddr, &cap.dstaddr);
    }
    cap.dstport = dstport;

    /* Open snoop */
    if (SYSERR == snoopOpen(&cap, devname))
    {
        fprintf(stderr, "Failed to open capture on network device '%s'\n",
                devname);
        return 1;
    }

    /* Spawn output thread */
    tid = create((void *)snoop, SHELL_CMDSTK, SHELL_CMDPRIO, "snoop",
                 4, &cap, count, dump, verbose);
    if (SYSERR == tid)
    {
        snoopClose(&cap);
        fprintf(stderr, "Failed to start capture\n");
        return 1;
    }

    /* Use same stdin, stdout, and sterr as this thread */
    thrtab[tid].fdesc[0] = thrtab[gettid()].fdesc[0];
    thrtab[tid].fdesc[1] = thrtab[gettid()].fdesc[1];
    thrtab[tid].fdesc[2] = thrtab[gettid()].fdesc[2];

    /* Start output thread */
    recvclr();
    ready(tid, RESCHED_NO);

    /* Close at proper event */
    if (0 == count)
    {
        /* Stop snooping when enter is hit */
        fprintf(stdout, "Snooping... Press <Enter> to stop.\n");
        getchar();
        kill(tid);
    }
    else
    {
        /* Stop snooping when thread dies */
        fprintf(stdout, "Snooping %d packets...\n", count);
        while (receive() != tid);
    }

    /* Print out statistics */
    printf("%d packets captured\n", cap.ncap);
    printf("%d packets matched filter\n", cap.nmatch);
    printf("%d packets printed\n", cap.nprint);
    printf("%d packets overrun\n", cap.novrn);

    /* Close interface */
    if (SYSERR == snoopClose(&cap))
    {
        fprintf(stderr, "Failed to stop capture\n");
        return 1;
    }

    return 0;

}
#endif /* NETHER */
