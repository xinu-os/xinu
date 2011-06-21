/**
 * @file     xsh_voip.c
 * @provides xsh_voip.
 *
 * $Id: xsh_voip.c 2116 2009-11-03 20:55:05Z zlund $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <device.h>
#include <udp.h>
#include <stdlib.h>
#include <shell.h>
#include <ipv4.h>
#include <network.h>
#include <ether.h>
#include <thread.h>
#include <clock.h>
#include <dsp.h>

#define UDP_PORT 22020

#define BUF_SIZE     1024
#define SEQ_BUF_SIZE 800

#define MODE_SERIAL 1
#define MODE_LOCAL 2
#define MODE_IP 3
#define CHECK_BASIC 1
#define CHECK_SEQ 2

/* Returns a timespan in ms */
#define timespan(curtime, curticks, oldtime, oldticks) \
    ((curtime * CLKTICKS_PER_SEC + curticks) - \
    (oldtime * CLKTICKS_PER_SEC + oldticks)) * \
    1000 / CLKTICKS_PER_SEC

struct voipPkt
{
    uint seq;
    uint len;
    uchar buf[SEQ_BUF_SIZE];
};

thread serial_loop(ushort uart);
thread basic_send(ushort uart, ushort udp);
thread basic_receive(ushort uart, ushort udp);
thread seq_send(ushort uart, ushort udp);
thread seq_receive(ushort uart, ushort udp);

/**
 * Shell command for VoIP functionality.
 * @param nargs number of arguments
 * @param args  array of arguments
 * @return OK for success, SYSERR for syntax error
 */
shellcmd xsh_voip(int nargs, char *args[])
{
    int i, rxPort = UDP_PORT, txPort = UDP_PORT;
    ushort uart = NULL, udpRx = NULL, udpTx = NULL;
    ushort mode = MODE_SERIAL, check = CHECK_BASIC;
    struct netaddr *localhost;
    struct netaddr host;
#ifdef ELOOP
    struct netaddr mask;
#endif
    struct netif *netptr;
    struct thrent *thrptr;

    if (nargs < 2)
    {
        fprintf(stderr, "%s: too few arguments\n", args[0]);
        fprintf(stderr, "Try '%s --help' for more information\n",
                args[0]);
        return SYSERR;
    }
    else if (nargs == 2 && strncmp(args[1], "--help", 6) == 0)
    {
        printf
            ("\nUsage: %s [-p <rx port> <tx port>] [-s] [serial] [localhost] [IP address]\n",
             args[0]);
        printf("Description:\n");
        printf("\tUses the second serial port and the network to ");
        printf("transmit and receive audio.\n");
        printf("Options:\n");
        printf
            ("\t-p\t\tSpecify UDP receive and transmit port numbers.\n");
        printf("\t-s\t\tEmbed sequence information in each packet.\n");
        printf("\tserial\t\tOperate in serial loopback mode.\n");
        printf("\tlocalhost\tOperate in network loopback mode.\n");
        printf("\tIP address\tSend and receive from this IP address.\n");
        return OK;
    }
    else
    {
        for (i = 1; i < nargs; i++)
        {
            if (strncmp(args[i], "-s", 2) == 0)
            {
                check = CHECK_SEQ;
            }
            else if (strncmp(args[i], "-p", 2) == 0)
            {
                if ((i + 2) < nargs)
                {
                    i++;
                    rxPort = atol(args[i]);
                    i++;
                    txPort = atol(args[i]);
                }
                else
                {
                    fprintf(stderr,
                            "Insufficient number of arguments for port numbers.\n");
                    return SYSERR;
                }
            }
            else if (strncmp(args[i], "serial", 6) == 0)
            {
                mode = MODE_SERIAL;
            }
            else if (strncmp(args[i], "localhost", 9) == 0)
            {
                mode = MODE_LOCAL;
            }
            else if (SYSERR != dot2ipv4(args[i], &host))
            {
                mode = MODE_IP;
            }
            else
            {
                fprintf(stderr,
                        "Failed to parse destination IP address.\n");
                return SYSERR;
            }
        }
    }

    enable();

#ifdef TTY1
    /* Kill the SHELL1 thread */
    for (i = 0; i < NTHREAD; i++)
    {
        thrptr = &thrtab[i];

        if (thrptr->state == THRFREE)
        {
            continue;
        }

        if (0 == strncmp(thrptr->name, "SHELL1", 6))
        {
            kill(i);
        }
    }

    /* Close TTY1 so we can use SERIAL1 */
    close(TTY1);
#else
    thrptr = NULL;
#endif                          /* TTY1 */

#ifdef SERIAL1
    uart = SERIAL1;
#else
    uart = SYSERR;
#endif                          /* SERIAL1 */
    if (SYSERR == (short)uart)
    {
        fprintf(stderr, "Failed to select a uart device.");
        return SYSERR;
    }

    switch (mode)
    {
    case MODE_SERIAL:
        ready(create
              ((void *)serial_loop, INITSTK, 20, "voip-serial", 1, uart),
              RESCHED_YES);
        return OK;
#ifdef ELOOP
    case MODE_LOCAL:
        if (SYSERR == open(ELOOP))
        {
            fprintf(stderr, "Failed to open loopback device.");
            return SYSERR;
        }
        dot2ipv4("127.0.0.1", &host);
        localhost = &host;
        dot2ipv4("255.255.255.0", &mask);
        if (SYSERR == netUp(ELOOP, &host, &mask, NULL))
        {
            close(ELOOP);
            fprintf(stderr,
                    "Failed to bring up network on loopback device.\n");
            return SYSERR;
        }
        break;
#endif                          /* ELOOP */
    case MODE_IP:
        netptr = netLookup((ethertab[0].dev)->num);
        if (NULL == netptr)
        {
            fprintf(stderr, "No network interface found.\n");
            return SYSERR;
        }
        localhost = &(netptr->ip);
        srand(localhost->addr[3]);      /* Seed the random number generator */
        break;
    default:
        return SYSERR;
    }

#ifdef NUDP
    if (UDP_PORT == rxPort && UDP_PORT == txPort)
    {
        /* If both ports are the default, use one UDP device */
        udpRx = udpAlloc();
        udpTx = udpRx;
    }
    else
    {
        /* Otherwise, use two UDP devices */
        udpRx = udpAlloc();
        udpTx = udpAlloc();
    }
#else
    udpRx = SYSERR;
    udpTx = SYSERR;
#endif                          /* NUDP */
    if (SYSERR == (short)udpRx || SYSERR == (short)udpTx)
    {
        fprintf(stderr, "Unable to allocate a UDP device.\n");
        return SYSERR;
    }

    if (udpRx == udpTx)
    {
        if (SYSERR == open(udpRx, localhost, &host, rxPort, txPort))
        {
            fprintf(stderr, "Failed to open UDP device.\n");
            return SYSERR;
        }
        printf("Opened a single UDP device.\n");
    }
    else
    {
        if (SYSERR == open(udpRx, localhost, NULL, rxPort, NULL))
        {
            fprintf(stderr, "Failed to open UDP device.\n");
            return SYSERR;
        }
        if (SYSERR == open(udpTx, localhost, &host, NULL, txPort))
        {
            fprintf(stderr, "Failed to open UDP device.\n");
            return SYSERR;
        }
        printf("Opened two UDP devices.\n");
    }

    switch (check)
    {
    case CHECK_BASIC:
        ready(create
              ((void *)basic_send, INITSTK, 20, "voip-send", 2, uart,
               udpTx), RESCHED_YES);
        ready(create
              ((void *)basic_receive, INITSTK, 20, "voip-receive", 2,
               uart, udpRx), RESCHED_YES);
        break;
    case CHECK_SEQ:
#ifdef NUDP
        control(udpRx, UDP_CTRL_SETFLAG, UDP_FLAG_NOBLOCK, NULL);
        control(udpTx, UDP_CTRL_SETFLAG, UDP_FLAG_NOBLOCK, NULL);
#endif                          /* NUDP */
        ready(create
              ((void *)seq_send, INITSTK, 20, "voip-send", 2, uart,
               udpTx), RESCHED_YES);
        ready(create
              ((void *)seq_receive, INITSTK, 20, "voip-receive", 2, uart,
               udpRx), RESCHED_YES);
        break;
    }
    return OK;
}

thread serial_loop(ushort uart)
{
    uint len = 0;
    uchar buf[BUF_SIZE];

    /* Enable all interrupts */
    enable();

    while (TRUE)
    {
        len = read(uart, buf, BUF_SIZE);
        if (len > 0)
        {
            write(uart, buf, len);
        }
    }
}

thread basic_send(ushort uart, ushort udp)
{
    uint len = 0;
    uchar buf[BUF_SIZE];

    /* Enable all interrupts */
    enable();

    while (TRUE)
    {
        /* Read from the serial device */
        len = read(uart, buf, BUF_SIZE);

        if (len > 0)
        {
            /* Write to the UDP device */
            write(udp, buf, len);
        }
    }
}

//#define ECHO
thread basic_receive(ushort uart, ushort udp)
{
    uint len = 0;
    uchar buf[BUF_SIZE];
#ifdef ECHO
    int i, j = 0;
    uint value[5 * BUF_SIZE];
#endif

    /* Enable all interrupts */
    enable();

    while (TRUE)
    {
        /* Read from the UDP device */
        len = read(udp, buf, BUF_SIZE);

        if (len > 0)
        {
#ifdef ECHO
            /* Echo audio effect */
            for (i = 0; i < len; i++)
            {
                value[j] =
                    value[(j + BUF_SIZE) % (5 * BUF_SIZE)] * 4 / 5 +
                    ulaw2linear(buf[i]) / 64 + 512;
                buf[i] = linear2ulaw((value[j] - 512) * 64);
                j = (j + 1) % (5 * BUF_SIZE);
            }
#endif

            /* Write to the serial device */
            write(uart, buf, len);
        }
    }
}

//#define DROP
thread seq_send(ushort uart, ushort udp)
{
#ifdef DROP
    int i = 0, skip, dup;
#endif
    struct voipPkt *voip;
    voip = malloc(sizeof(struct voipPkt));

    enable();
    voip->seq = 0;

#ifdef DROP
    skip = rand() % 100;
    dup = rand() % 100;
#endif

    while (TRUE)
    {
        /* Read from the serial device */
        voip->len = read(uart, voip->buf, SEQ_BUF_SIZE);
        if (voip->len > 0)
        {
            /* Write to the UDP device */
#ifdef DROP
            if (i < skip)
            {
#endif
                write(udp, voip, sizeof(struct voipPkt));
#ifdef DROP
                if (i == dup)
                {
                    write(udp, voip, sizeof(struct voipPkt));
                    dup = rand() % 100;
                }
                i++;
            }
            else
            {
                kprintf("d%d ", voip->seq);
                i = 0;
                skip = rand() % 100;
            }
#endif
            voip->seq++;
        }
        resched();
    }
}

thread seq_receive(ushort uart, ushort udp)
{
    uint len, seq = 0;
    ulong ticks, time;
    struct voipPkt *voip;
    voip = malloc(sizeof(struct voipPkt));

    voip->len = 0;

    enable();

    while (TRUE)
    {
        /* Read from the UDP device */
        len = read(udp, voip, sizeof(struct voipPkt));
        if (len > 0)
        {
            if (seq < voip->seq)
            {
                kprintf("l%d ", seq);   //voip->seq + 1);
                seq = voip->seq;
            }
            else if (voip->seq < seq)
            {
                kprintf("dup%d", voip->seq);
                continue;
            }
            seq++;
            //kprintf("%d ms\r\n", timespan(clktime, clkticks, time, ticks));
            ticks = clkticks;
            time = clktime;
            /* Write to the serial device */
            write(uart, voip->buf, voip->len);
        }
        else if (timespan(clktime, clkticks, time, ticks) >
                 (SEQ_BUF_SIZE / 8))
        {
            ticks = clkticks;
            time = clktime;
            write(uart, voip->buf, voip->len);
            kprintf("- ");
        }
        resched();
    }
}
