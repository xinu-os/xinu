/*
 * @file     xsh_ping.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread.h>
#include <string.h>
#include <network.h>
#include <ipv4.h>
#include <icmp.h>
#include <clock.h>
#include <shell.h>
#include <interrupt.h>
#include <platform.h>

#if NETHER
static int echoQueueAlloc(void);
static int echoQueueDealloc(int echoent);
static struct packet *echoQueueGet(int echoent);
static void echoPrintPkt(struct packet *pkt, ulong elapsed);
static ulong echoTripTime(struct packet *pkt);

/**
 * @ingroup shell
 *
 * Shell command (ping).
 * @param nargs  number of arguments in args array
 * @param args   array of arguments
 * @return OK for success, SYSERR for syntax error
 */
shellcmd xsh_ping(int nargs, char *args[])
{
    int i = 0;
    int interval = 1000, count = 10, recv = 0, echoq = 0;
    ulong rtt = 0, min = 0, max = 0, total = 0;
    ulong startsec = 0, startms = 0;
    struct netaddr target;
    struct packet *pkt = NULL;
    char str[50];

    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strcmp(args[1], "--help") == 0)
    {
        printf("Usage: ping <IP>\n\n");
        printf("Description:\n");
        printf("\tSend ICMP echo requests to network hosts\n");
        printf("Options:\n");
        printf("\t<IP>\t\tIP address\n");
        printf("\t-c count\tstop after sending count packets\n");
        printf
            ("\t-i interval\tsleep interval milliseconds between pings\n");
        printf("\t--help\t\tdisplay this help and exit\n");
        return OK;
    }

    /* Check for correct number of arguments */
    if (nargs < 2)
    {
        fprintf(stderr, "ping: too few arguments\n");
        fprintf(stderr, "Try 'ping --help' for more information\n");
        return SHELL_ERROR;
    }

    i = 1;
    while (i < nargs)
    {
        if (0 == strcmp(args[i], "-c"))
        {
            i++;
            if (i < nargs)
            {
                count = atoi(args[i]);
            }
            else
            {
                fprintf(stderr, "ping: -c requires integer argument\n");
                return SHELL_ERROR;
            }
        }
        else if (0 == strcmp(args[i], "-i"))
        {
            i++;
            if (i < nargs)
            {
                interval = atoi(args[i]);
            }
            else
            {
                fprintf(stderr, "ping: -i requires integer argument\n");
                return SHELL_ERROR;
            }
        }
        else if (SYSERR == dot2ipv4(args[i], &target))
        {
            fprintf(stderr, "ping: %s is not a valid IPv4 address.\n",
                    args[i]);
            return SHELL_ERROR;
        }
        i++;
    }

    netaddrsprintf(str, &target);
    if (0 == strcmp(str, "ERROR"))
    {
        fprintf(stderr, "ping: destination IP address required.\n");
        return SHELL_ERROR;
    }
    printf("PING %s\n", str);

    echoq = echoQueueAlloc();
    if (SYSERR == echoq)
    {
        printf("...No free echo queues!\n");
        return SHELL_ERROR;
    }

    startsec = clktime;
    startms = clkticks;

    for (i = 0; i < count; i++)
    {
        // Send ping packet
        if (SYSERR == icmpEchoRequest(&target, gettid(), i))
        {
            printf("...Failed to reach %s\n", str);
            return SHELL_ERROR;
        }

        sleep(interval);
        if (NOMSG != recvclr())
        {
            // pick reply packets off of the queue
            pkt = echoQueueGet(echoq);
            while ((NULL != (ulong)pkt) && (SYSERR != (ulong)pkt))
            {
                rtt = echoTripTime(pkt);
                total += rtt;
                if ((rtt < min) || (0 == min))
                {
                    min = rtt;
                }
                if (rtt > max)
                {
                    max = rtt;
                }

                echoPrintPkt(pkt, rtt);
                netFreebuf(pkt);
                recv++;
                pkt = echoQueueGet(echoq);
            }
        }
    }
    echoQueueDealloc(echoq);

    netaddrsprintf(str, &target);
    printf("--- %s ping statistics ---\n", str);
    printf("%d packets transmitted, %d received,", count, recv);
    printf(" %d%% packet loss,", (count - recv) * 100 / count);
    printf(" time %lums\n", (clktime - startsec) * 1000 +
           ((clkticks - startms) * 1000) / CLKTICKS_PER_SEC);
    printf("rtt min/avg/max = %lu.%03lu/", min / 1000, min % 1000);
    if (0 != recv)
    {
        printf("%lu.%03lu/", (total / recv) / 1000, (total / recv) % 1000);
    }
    else
    {
        printf("-/");
    }
    printf("%lu.%03lu ms\n", max / 1000, max % 1000);

    return SHELL_OK;
}

/**
 * Search for an echo reply queue that is not in use.
 */
static int echoQueueAlloc(void)
{
    irqmask im;
    int i = 0;

    im = disable();
    for (i = 0; i < NPINGQUEUE; i++)
    {
        if (BADTID == echotab[i].tid)
        {
            echotab[i].tid = gettid();
            restore(im);
            return i;
        }
    }

    restore(im);
    return SYSERR;
}

/**
 * Give up an echo reply queue, and flush any loitering packets.
 */
static int echoQueueDealloc(int echoent)
{
    irqmask im;
    struct packet *pkt = NULL;
    struct icmpEchoQueue *eq = NULL;

    if ((echoent < 0) || (echoent >= NPINGQUEUE))
    {
        return SYSERR;
    }
    eq = &echotab[echoent];

    im = disable();
    eq->tid = BADTID;
    while (eq->tail != eq->head)
    {
        pkt = eq->pkts[eq->tail];
        ICMP_TRACE("Discarding ping packet from closed queue %d",
                   echoent);
        netFreebuf(pkt);
        eq->tail = (eq->tail + 1) % NPINGHOLD;
    }

    restore(im);
    return OK;
}

/**
 * Fetch a packet from an echo reply queue.
 */
static struct packet *echoQueueGet(int echoent)
{
    irqmask im;
    struct packet *pkt = NULL;
    struct icmpEchoQueue *eq = NULL;

    if ((echoent < 0) || (echoent >= NPINGQUEUE))
    {
        return (struct packet *)SYSERR;
    }
    eq = &echotab[echoent];

    im = disable();

    if (eq->tid != gettid())
    {
        restore(im);
        return (struct packet *)SYSERR;
    }

    if (eq->head == eq->tail)
    {
        restore(im);
        return (struct packet *)NULL;
    }
    pkt = eq->pkts[eq->tail];
    eq->tail = (eq->tail + 1) % NPINGHOLD;

    restore(im);
    return pkt;
}

/**
 * Print out a echo reply packet.
 */
static void echoPrintPkt(struct packet *pkt, ulong elapsed)
{
    struct icmpPkt *icmp = NULL;
    struct icmpEcho *echo = NULL;
    struct ipv4Pkt *ip = NULL;
    struct netaddr src;
    char str[50];

    ip = (struct ipv4Pkt *)pkt->nethdr;
    icmp = (struct icmpPkt *)pkt->curr;
    echo = (struct icmpEcho *)icmp->data;

    src.type = NETADDR_IPv4;
    src.len = IPv4_ADDR_LEN;
    memcpy(src.addr, ip->src, src.len);

    netaddrsprintf(str, &src);
    printf("%d bytes from %s: ", net2hs(ip->len), str);
    printf("icmp_seq=%d ttl=%d ", net2hs(echo->seq), ip->ttl);
    printf("time=%lu.%03lu ms\n", elapsed / 1000, elapsed % 1000);
}

/**
 * Return elapsed trip time in microseconds.
 */
static ulong echoTripTime(struct packet *pkt)
{
    struct icmpPkt *icmp = NULL;
    struct icmpEcho *echo = NULL;
    ulong elapsed = 0, cycPerTick = 0;

    icmp = (struct icmpPkt *)pkt->curr;
    echo = (struct icmpEcho *)icmp->data;

    cycPerTick = platform.clkfreq / CLKTICKS_PER_SEC;

    elapsed = (echo->arrivcyc - net2hl(echo->timecyc)) % cycPerTick;
    elapsed += (echo->arrivtic - net2hl(echo->timetic)) * cycPerTick;
    elapsed +=
        (echo->arrivsec - net2hl(echo->timesec)) * platform.clkfreq;
    elapsed = elapsed / (platform.clkfreq / 1000000);
    return elapsed;
}
#endif /* NETHER */
