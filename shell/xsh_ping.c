/*
 * @file xsh_ping.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <conf.h>

#if NETHER

#include <clock.h>
#include <icmp.h>
#include <interrupt.h>
#include <ipv4.h>
#include <limits.h>
#include <network.h>
#include <platform.h>
#include <shell.h>
#include <stdio.h>
#include <string.h>
#include <thread.h>

static int echoQueueAlloc(void);
static void echoQueueDealloc(int echoent);
static struct packet *echoQueueGet(int echoent);
static void echoPrintPkt(const struct packet *pkt, ulong elapsed);
static ulong echoTripTime(const struct packet *pkt);
static ulong tickDiff(ulong startsec, ulong startticks,
                      ulong endsec, ulong endticks);

/**
 * @ingroup shell
 *
 * Shell command (ping).
 *
 * @param nargs  number of arguments in args array
 * @param args   array of arguments
 *
 * @return
 *      ::SHELL_OK if successful; ::SHELL_ERROR if there was a syntax error or
 *      any echo requests could not be sent.  (Failure to receive echo replies
 *      is not considered a cause to return ::SHELL_ERROR.)
 */
shellcmd xsh_ping(int nargs, char *args[])
{
    uint i;
    uint interval = 1000;
    uint intervalticks;
    uint count = 10;
    uint num_recv = 0;
    int echoq;
    ulong min_rtt = ULONG_MAX, max_rtt = 0, total_rtt = 0;
    ulong startsec, startticks;
    ulong endsec, endticks;
    struct netaddr target;
    char target_str[50];
    irqmask im;

    /* Output help if '--help' argument was supplied.  */
    if (2 == nargs && 0 == strcmp(args[1], "--help"))
    {
        printf("Usage: ping <IP>\n\n");
        printf("Description:\n");
        printf("\tSend ICMP echo requests to network hosts\n");
        printf("Options:\n");
        printf("\t<IP>\t\tIP address\n");
        printf("\t-c count\tstop after sending count packets\n");
        printf("\t-i interval\tsleep interval milliseconds between pings\n");
        printf("\t--help\t\tdisplay this help and exit\n");
        return SHELL_OK;
    }

    for (i = 1; i < nargs; i++)
    {
        if (0 == strcmp(args[i], "-c"))
        {
            char tmp;

            i++;
            if (i == nargs ||
                1 != sscanf(args[i], "%u%c", &count, &tmp) ||
                0 >= count)
            {
                fprintf(stderr, "ping: -c requires positive integer argument\n");
                return SHELL_ERROR;
            }
        }
        else if (0 == strcmp(args[i], "-i"))
        {
            char tmp;

            i++;
            if (i == nargs ||
                1 != sscanf(args[i], "%u%c", &interval, &tmp))
            {
                fprintf(stderr, "ping: -i requires integer argument\n");
                return SHELL_ERROR;
            }
        }
        else
        {
            break;
        }
    }

    if (i == nargs)
    {
        fprintf(stderr, "ping: destination IP address required.\n");
        return SHELL_ERROR;
    }

    if (i < nargs - 1)
    {
        fprintf(stderr, "ping: only one destination IP address is allowed.\n");
        return SHELL_ERROR;
    }

    if (SYSERR == dot2ipv4(args[i], &target))
    {
        fprintf(stderr, "ping: %s is not a valid IPv4 address.\n", args[i]);
        return SHELL_ERROR;
    }

    netaddrsprintf(target_str, &target);

    printf("PING %s\n", target_str);

    echoq = echoQueueAlloc();
    if (SYSERR == echoq)
    {
        printf("...No free echo queues!\n");
        return SHELL_ERROR;
    }

    im = disable();
    startsec = clktime;
    startticks = clkticks;
    restore(im);

    intervalticks = interval * CLKTICKS_PER_SEC / 1000;

    for (i = 0; i < count; i++)
    {
        ulong sendsec, sendticks;

        // Send ping packet
        if (OK != icmpEchoRequest(&target, gettid(), i))
        {
            printf("...Failed to reach %s\n", target_str);
            echoQueueDealloc(echoq);
            return SHELL_ERROR;
        }

        im = disable();
        sendsec = clktime;
        sendticks = clkticks;
        restore(im);

        // Wait for response
        if (TIMEOUT != recvtime(intervalticks))
        {
            struct packet *pkt;

            // pick reply packets off of the queue
            while (NULL != (pkt = echoQueueGet(echoq)))
            {
                ulong rtt;

                rtt = echoTripTime(pkt);
                total_rtt += rtt;
                min_rtt = min(min_rtt, rtt);
                max_rtt = max(max_rtt, rtt);
                echoPrintPkt(pkt, rtt);
                netFreebuf(pkt);
                num_recv++;
            }

            // Sleep for remaining time
            if (i < count - 1)
            {
                ulong elapsedticks;
                ulong recvsec, recvticks;

                im = disable();
                recvsec = clktime;
                recvticks = clkticks;
                restore(im);

                elapsedticks = tickDiff(sendsec, sendticks, recvsec, recvticks);

                if (elapsedticks < intervalticks)
                {
                    sleep((intervalticks - elapsedticks) * 1000 / CLKTICKS_PER_SEC);
                }
            }
        }
    }

    echoQueueDealloc(echoq);

    if (min_rtt == ULONG_MAX)
    {
        min_rtt = 0;
    }

    im = disable();
    endsec = clktime;
    endticks = clkticks;
    restore(im);

    printf("--- %s ping statistics ---\n", target_str);
    printf("%u packets transmitted, %u received,", count, num_recv);
    printf(" %u%% packet loss,", (count - num_recv) * 100 / count);
    printf(" time %lums\n",
           tickDiff(startsec, startticks,
                    endsec, endticks) * 1000 / CLKTICKS_PER_SEC);
    printf("rtt min/avg/max = %lu.%03lu/", min_rtt / 1000, min_rtt % 1000);
    if (0 != num_recv)
    {
        printf("%lu.%03lu/", (total_rtt / num_recv) / 1000,
               (total_rtt / num_recv) % 1000);
    }
    else
    {
        printf("-/");
    }
    printf("%lu.%03lu ms\n", max_rtt / 1000, max_rtt % 1000);

    return SHELL_OK;
}

/* Allocate an ICMP Echo Reply queue.  Return index of allocated queue, or
 * SYSERR if none are available.  */
static int echoQueueAlloc(void)
{
    irqmask im;
    int i;
    int retval;

    im = disable();
    retval = SYSERR;

    for (i = 0; i < NPINGQUEUE; i++)
    {
        if (BADTID == echotab[i].tid)
        {
            echotab[i].tid = gettid();
            retval = i;
            break;
        }
    }

    restore(im);
    return retval;
}

/* Release an echo reply queue, freeing any buffered packets.  */
static void echoQueueDealloc(int echoent)
{
    irqmask im;
    struct packet *pkt;
    struct icmpEchoQueue *eq;

    eq = &echotab[echoent];

    im = disable();
    eq->tid = BADTID;
    while (eq->tail != eq->head)
    {
        pkt = eq->pkts[eq->tail];
        ICMP_TRACE("Discarding ping packet from closed queue %d", echoent);
        netFreebuf(pkt);
        eq->tail = (eq->tail + 1) % NPINGHOLD;
    }

    restore(im);
}

/* Fetch a packet from an ICMP Echo Reply queue.  Returns a pointer to the
 * resulting packet buffer, or NULL if no packets are available in the specified
 * queue.  */
static struct packet *echoQueueGet(int echoent)
{
    irqmask im;
    struct packet *pkt;
    struct icmpEchoQueue *eq;

    eq = &echotab[echoent];
    im = disable();
    if (eq->head != eq->tail)
    {
        pkt = eq->pkts[eq->tail];
        eq->tail = (eq->tail + 1) % NPINGHOLD;
    }
    else
    {
        pkt = NULL;
    }

    restore(im);
    return pkt;
}

/* Prints information from an ICMP Echo Reply packet.
 *
 * @elapsed specifies the number of microseconds that have elapsed since the
 * corresponding ICMP Echo Request was sent.  */
static void echoPrintPkt(const struct packet *pkt, ulong elapsed)
{
    const struct ipv4Pkt *ip;
    const struct icmpPkt *icmp;
    const struct icmpEcho *echo;
    struct netaddr src;
    char src_str[50];

    ip = (const struct ipv4Pkt *)pkt->nethdr;
    icmp = (const struct icmpPkt *)pkt->curr;
    echo = (const struct icmpEcho *)icmp->data;

    src.type = NETADDR_IPv4;
    src.len = IPv4_ADDR_LEN;
    memcpy(src.addr, ip->src, src.len);

    netaddrsprintf(src_str, &src);
    printf("%d bytes from %s: ", net2hs(ip->len), src_str);
    printf("icmp_seq=%d ttl=%d ", net2hs(echo->seq), ip->ttl);
    printf("time=%lu.%03lu ms\n", elapsed / 1000, elapsed % 1000);
}

/*
 * Return the elapsed round trip time of an ICMP echo request and reply, in
 * microseconds.
 *
 * This relies on the timecyc, timetic, and timesec fields that were stored in
 * the echo request and echoed back, as well as the arrivcyc, arrivtic, and
 * arrivsec fields which were set by icmpRecv().
 */
static ulong echoTripTime(const struct packet *pkt)
{
    const struct icmpPkt *icmp;
    const struct icmpEcho *echo;
    ulong cycPerTick;
    ulong elapsedticks, elapsedcyc;

    icmp = (const struct icmpPkt *)pkt->curr;
    echo = (const struct icmpEcho *)icmp->data;

    cycPerTick = platform.clkfreq / CLKTICKS_PER_SEC;

    elapsedticks = tickDiff(net2hl(echo->timesec), net2hl(echo->timetic),
                            echo->arrivsec, echo->arrivtic);

    elapsedcyc = elapsedticks * cycPerTick;

    elapsedcyc += (echo->arrivcyc - net2hl(echo->timecyc)) % cycPerTick;

    return elapsedcyc / (platform.clkfreq / 1000000);
}

/* Returns the number of timer ticks that have elapsed between two times.
 *
 * NOTE: This function is needed because tick wraparound occurs before
 * wraparound in the underlying data type and therefore needs special handling.
 */
static ulong tickDiff(ulong startsec, ulong startticks,
                      ulong endsec, ulong endticks)
{
    ulong elapsedticks;

    elapsedticks = (endsec - startsec) * CLKTICKS_PER_SEC;

    if (endticks >= startticks)
    {
        elapsedticks += (endticks - startticks);
    }
    else
    {
        elapsedticks -= (startticks - endticks);
    }

    return elapsedticks;
}

#endif /* NETHER */
