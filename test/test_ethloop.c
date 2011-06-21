#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <testsuite.h>
#include <network.h>
#include <device.h>
#include <memory.h>
#include <string.h>
#include <ethloop.h>

static int ethloopn_test(bool verbose, int dev);

#define MAX_PAYLOAD  1516
#define ETH_TYPE_ARP 0x0806

#ifndef NETHLOOP
#define NETHLOOP (-1)
#endif

#ifndef ELOOP
#define ELOOP (-1)
#endif

/* Simple ethernet packet structure */
struct etherGram
{
    char dst[ETH_ADDR_LEN];     /* Destination MAC */
    char src[ETH_ADDR_LEN];     /* Source MAC */
    ushort type_len;            /* EthernetII type/Ethernet length */
    char payload[1];            /* Payload data */
};

/**
 * Test for ethernet loopback (ethloop) driver packet acceptance.
 */
thread test_ethloop(bool verbose)
{
    /* the failif macro depends on 'passed' and 'verbose' vars */
    bool passed = TRUE;
    int i;

    enable();

    for (i = 0; i < NETHLOOP; i++)
    {
        passed &= ethloopn_test(verbose, i + ELOOP);
    }

    /* always print out the overall tests status */
    if (passed)
    {
        testPass(TRUE, "");
    }
    else
    {
        testFail(TRUE, "");
    }

    return OK;
}

static int ethloopn_test(bool verbose, int dev)
{
    bool passed = TRUE;
    bool subpass;
    uint memsize;
    int i, value, len;
    struct etherGram *inpkt;
    struct etherGram *outpkt;
    char *payload;
    char str[80];
    int devminor;
    struct ethloop *pelp;
    struct netaddr addr;
    device *pdev;

    pdev = (device *)&devtab[dev];
    devminor = pdev->minor;
    pelp = &elooptab[devminor];

    sprintf(str, "%s Open", pdev->name);
    testPrint(verbose, str);
    failif((SYSERR == open(dev)), "");

    /* memget */
    memsize = sizeof(struct etherGram) + MAX_PAYLOAD - 1;
    inpkt = memget(memsize);
    outpkt = memget(memsize);

    payload = &(outpkt->payload[0]);

    control(dev, NET_GET_HWADDR, (int)&addr, NULL);
    memcpy(outpkt->dst, addr.addr, addr.len);
    memcpy(outpkt->src, addr.addr, addr.len);
    outpkt->type_len = hs2net(ETH_TYPE_ARP);

    /* generate payload content */
    for (i = 0; i < MAX_PAYLOAD + 1; i++)
    {
        /* Cycle through 0x20 to 0x7d (range of 0x5e) */
        value = (i % 0x5e) + 0x20;
        payload[i] = value;
    }

    /* oversized packet (paylod 1502 bytes + 14 byte header) */
    sprintf(str, "%s 1516 byte packet", pelp->dev->name);
    testPrint(verbose, str);
    len = write(dev, outpkt, 1516);
    failif((SYSERR != len), "");

    /* max packet (payload 1500 bytes + 14 byte header) */
    sprintf(str, "%s 1514 byte packet (write)", pelp->dev->name);
    testPrint(verbose, str);
    len = write(dev, outpkt, 1514);
    failif((len < 1514), "");

    sprintf(str, "%s 1514 byte packet (read)", pelp->dev->name);
    testPrint(verbose, str);
    bzero(inpkt, memsize);
    len = read(dev, inpkt, 1514);
    failif((0 != memcmp(outpkt, inpkt, 1514)), "");

    /* 'normal' packet (payload 686 bytes + 14 byte header) */
    sprintf(str, "%s  700 byte packet (write)", pelp->dev->name);
    testPrint(verbose, str);
    len = write(dev, outpkt, 700);
    failif((len < 700), "");

    sprintf(str, "%s  700 byte packet (read)", pelp->dev->name);
    testPrint(verbose, str);
    bzero(inpkt, memsize);
    len = read(dev, inpkt, 700);
    failif((0 != memcmp(outpkt, inpkt, 700)), "");

    /* small packet (payload 16 bytes + 14 byte header) */
    sprintf(str, "%s   30 byte packet (write)", pelp->dev->name);
    testPrint(verbose, str);
    len = write(dev, outpkt, 30);
    failif((len < 30), "");

    sprintf(str, "%s   30 byte packet (read)", pelp->dev->name);
    testPrint(verbose, str);
    bzero(inpkt, memsize);
    len = read(dev, inpkt, 30);
    failif((0 != memcmp(outpkt, inpkt, 30)), "");

    /* micro packet (12 bytes) */
    sprintf(str, "%s   12 byte packet", pelp->dev->name);
    testPrint(verbose, str);
    len = write(dev, outpkt, 12);
    failif((SYSERR != len), "");

    /* send 512 random sized packets */
    sprintf(str, "%s  512 random-sized packets", pelp->dev->name);
    testPrint(verbose, str);
    subpass = TRUE;

    for (i = 0; i < 512; i++)
    {
        len = 32 + (rand() % 1200);
        value = write(dev, outpkt, len);
        if (value < len)
        {
            subpass = FALSE;
        }

        bzero(inpkt, memsize);
        value = read(dev, inpkt, len);
        if (0 != memcmp(outpkt, inpkt, 30))
        {
            subpass = FALSE;
        }
    }
    failif((TRUE != subpass), "");

    /* hold packet (payload 686 bytes + 14 byte header) */
    control(dev, ELOOP_CTRL_SETFLAG, ELOOP_FLAG_HOLDNXT, NULL);
    sprintf(str, "%s  700 byte packet (write hold)", pelp->dev->name);
    testPrint(verbose, str);
    len = write(dev, outpkt, 700);
    failif((len < 700), "");

    sprintf(str, "%s  700 byte packet (read hold)", pelp->dev->name);
    testPrint(verbose, str);
    bzero(inpkt, memsize);
    len = control(dev, ELOOP_CTRL_GETHOLD, (int)inpkt, 700);
    failif((0 != memcmp(outpkt, inpkt, 700)), "");

    /* drop packet (payload 686 bytes + 14 byte header) */
    control(dev, ELOOP_CTRL_SETFLAG, ELOOP_FLAG_DROPNXT, NULL);
    sprintf(str, "%s  700 byte packet (write drop)", pelp->dev->name);
    testPrint(verbose, str);
    len = write(dev, outpkt, 700);
    failif((len < 700), "");

    sprintf(str, "%s  700 byte packet (write)", pelp->dev->name);
    testPrint(verbose, str);
    outpkt->dst[0] += 1;
    len = write(dev, outpkt, 700);
    failif((len < 700), "");

    sprintf(str, "%s  700 byte packet (read)", pelp->dev->name);
    testPrint(verbose, str);
    bzero(inpkt, memsize);
    len = read(dev, inpkt, 700);
    failif((0 != memcmp(outpkt, inpkt, 700)), "");

    /* memfree */
    memfree(outpkt, memsize);
    memfree(inpkt, memsize);

    sprintf(str, "%s Close", pdev->name);
    testPrint(verbose, str);
    failif((SYSERR == close(dev)), "");

    return passed;
}
