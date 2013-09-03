/**
 * @file     xsh_pktgen.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <shell.h>

#include <clock.h>
#include <ether.h>
#include <device.h>

#include <ethernet.h>
#include <ipv4.h>
#include <udp.h>

#if NETHER
/* pktgen defaults */
#define DEF_INTERVAL 0
#define DEF_COUNT 0
#define DEF_DSTIP "192.168.1.1"
#define DEF_SRCIP "192.168.1.254"
#define DEF_DSTPT 1
#define DEF_SRCPT 65535
#define DEF_MINLEN 60
#define DEF_MAXLEN 1518

/* structure with pktgen request and tracking values */
struct pktgen_info
{
    /* device id to use for generator */
    int dev_id;
    /* L2 */
    uchar dstmac[ETH_ADDR_LEN];
    /* L3 */
    struct netaddr dstip;
    struct netaddr srcip;
    uchar l3_proto;
    /* L4 */
    ushort dstpt;
    ushort srcpt;

    /* other */
    uint minsize;
    uint maxsize;
    uint pktcount;
    uint interval;

    /* stats */
    uint start;
    uint stop;
    uint tries;
    uint errors;
};

thread pktgen(struct pktgen_info *);

static void usage(char *prog)
{
    printf("usage: %s [options] <iface> <dst-mac>\n", prog);
    printf("\t<iface>        interface to send packets on\n");
    printf("\t<dst-mac>      MAC address to send packets to\n");
    printf("\n");
    printf("options (and their [defaults]):\n");
    printf("\t-c <count>     number of packets to send [%d]\n",
           DEF_COUNT);
    printf("\t-i <interval>  milliseconds between packet sends [%d]\n",
           DEF_INTERVAL);
    printf("\t-h <dst-ip>    destination IP for header [%s]\n",
           DEF_DSTIP);
    printf("\t-H <src-ip>    source IP for header [%s]\n", DEF_SRCIP);
    printf("\t-p <dst-port>  destination port for header [%d]\n",
           DEF_DSTPT);
    printf("\t-P <src-port>  source port for header [%d]\n", DEF_SRCPT);
    printf("\t-l <min-length> minimum packet size [%d]\n", DEF_MINLEN);
    printf("\t-L <max-length> maximum packet size [%d]\n", DEF_MAXLEN);
}

/**
 * @ingroup shell
 *
 * pktgen lets a "user" start up a slightly parameterized packet generator
 * from the shell.  In theory it should be able to saturuate the full 100
 * Mbps, in reality it probably doesn't but it'll try its best.
 * @param nargs number of arguments
 * @param args  array of arguments
 * @return non-zero value on error
 */
shellcmd xsh_pktgen(int nargs, char *args[])
{
    int arg;
    tid_typ tid;
    struct pktgen_info info;
    uint count, interval;
    char *prog = args[0];
    char *dstip, *srcip;
    ushort dstpt, srcpt;
    uint minlen, maxlen;

    /* defaults */
    interval = DEF_INTERVAL;
    count = DEF_COUNT;
    dstip = DEF_DSTIP;
    srcip = DEF_SRCIP;
    dstpt = DEF_DSTPT;
    srcpt = DEF_SRCPT;
    minlen = DEF_MINLEN;
    maxlen = DEF_MAXLEN;

    /* parse args */
    struct getopt opts;
    opts.optreset = TRUE;
    while ((arg = getopt(nargs, args, "c:i:h:H:p:P:l:L:", &opts)) != -1)
    {
        switch (arg)
        {
        case 'c':
            count = atoi(opts.optarg);
            break;
        case 'i':
            interval = atoi(opts.optarg);
            break;
        case 'h':
            dstip = opts.optarg;
            break;
        case 'H':
            srcip = opts.optarg;
            break;
        case 'p':
            dstpt = atoi(opts.optarg);
            break;
        case 'P':
            srcpt = atoi(opts.optarg);
            break;
        case 'l':
            minlen = atoi(opts.optarg);
            break;
        case 'L':
            maxlen = atoi(opts.optarg);
            break;
        default:
            usage(prog);
            return 1;
        }
    }
    nargs -= opts.optind;
    args += opts.optind;

    /* grab the mac addr */
    if (2 != nargs)
    {
        usage(prog);
        return 1;
    }

    /* prep the args */
    info.dev_id = getdev(args[0]);
    colon2mac(args[1], info.dstmac);

    dot2ipv4(dstip, &info.dstip);
    dot2ipv4(srcip, &info.srcip);
    info.l3_proto = IPv4_PROTO_UDP;
    info.dstpt = dstpt;
    info.srcpt = srcpt;
    info.minsize = minlen;
    info.maxsize = maxlen;
    info.pktcount = count;
    info.interval = interval;

    /* spawn proper pktgen thread */
    tid = create(pktgen, INITSTK, INITPRIO, "pktgen", 1, &info);
    ready(tid, RESCHED_NO);

    /* listen for keypress to force stop */
    printf("Press enter/return to stop.\n");
    getchar();
    kill(tid);

    if (0 == info.stop)
    {
        info.stop = clktime;
    }

    /* print some stats about what we just did */
    printf("Tried to send %d packets (%d errors) over %d seconds.\n",
           info.tries, info.errors, info.stop - info.start);

    return 0;
}

thread pktgen(struct pktgen_info *info)
{
    struct packet *pkt;
    uchar *data;
    struct etherPkt *ethhdr;
    struct ipv4Pkt *iphdr;
    struct udpPkt *udphdr;

    /* start the clock */
    info->start = clktime;
    info->stop = 0;
    info->tries = 0;
    info->errors = 0;

    while (info->pktcount == 0 || info->tries < info->pktcount)
    {
        /* update the counter */
        info->tries += 1;

        /* prepare the packet */
        pkt = netGetbuf();
        if (SYSERR == (int)pkt)
        {
            info->errors += 1;
            continue;
        }

        /* take some liberties with headers (ETH, IP, UDP) */
        pkt->len =
            info->minsize +
            ((info->tries - 1) % (info->maxsize - info->minsize));
        pkt->curr -= (pkt->len + 03) & ~03;

        /* push on headers one-by-one */
        data = pkt->curr;

        /* Ethernet */
        ethhdr = (struct etherPkt *)data;
        memcpy(ethhdr->dst, info->dstmac, ETH_ADDR_LEN);
        control(info->dev_id, ETH_CTRL_GET_MAC, (long)ethhdr->src, NULL);
        ethhdr->type = ETHER_TYPE_IPv4;
        data += sizeof(*ethhdr);

        /* IP */
        iphdr = (struct ipv4Pkt *)data;
        // this looks magic, it is not.
        iphdr->ver_ihl = (IPv4_VERSION << 4) | (sizeof(*iphdr) >> 2);;
        iphdr->tos = IPv4_TOS_ROUTINE;
        iphdr->len = hs2net(pkt->len - sizeof(*ethhdr));
        iphdr->id = 0;
        iphdr->flags_froff = 0;
        iphdr->ttl = IPv4_TTL;
        iphdr->proto = info->l3_proto;
        iphdr->chksum = 0;
        memcpy(iphdr->src, info->srcip.addr, IPv4_ADDR_LEN);
        memcpy(iphdr->dst, info->dstip.addr, IPv4_ADDR_LEN);
        iphdr->chksum = netChksum(iphdr, IPv4_HDR_LEN);
        data += sizeof(*iphdr);

        if (IPv4_PROTO_UDP == info->l3_proto)
        {
            /* UDP */
            udphdr = (struct udpPkt *)data;
            udphdr->srcPort = hs2net(info->srcpt);
            udphdr->dstPort = hs2net(info->dstpt);
            udphdr->len =
                hs2net(pkt->len - sizeof(*ethhdr) - sizeof(*iphdr));
            udphdr->chksum = 0;
            data += sizeof(*udphdr);
        }

        /* send the packet -- don't care about routing, right to the ether! */
        if (SYSERR == write(info->dev_id, pkt->curr, pkt->len))
        {
            /* count the errors */
            info->errors += 1;
        }
        netFreebuf(pkt);
        sleep(info->interval);
    }

    /* stop the clock */
    info->stop = clktime;

    return 0;
}
#endif /* NETHER */
