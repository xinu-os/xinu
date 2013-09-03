/**
 * @file     test_netaddr.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <ether.h>
#include <interrupt.h>
#include <ipv4.h>
#include <network.h>
#include <testsuite.h>

thread test_netaddr(bool verbose)
{
#if NETHER
    bool passed = TRUE;
    struct netaddr a;
    struct netaddr b;

    /* Setup structures */
    a.type = NETADDR_ETHERNET;
    a.len = ETH_ADDR_LEN;
    a.addr[0] = 0xAA;
    a.addr[1] = 0xBB;
    a.addr[2] = 0xCC;
    a.addr[3] = 0xDD;
    a.addr[4] = 0xEE;
    a.addr[5] = 0xFF;

    b.type = NETADDR_ETHERNET;
    b.len = ETH_ADDR_LEN;
    b.addr[0] = 0xAA;
    b.addr[1] = 0xBB;
    b.addr[2] = 0xCC;
    b.addr[3] = 0xDD;
    b.addr[4] = 0xEE;
    b.addr[5] = 0xFF;

    testPrint(verbose, "Comparison (diff lengths)");
    b.len = ETH_ADDR_LEN - 1;
    failif((TRUE == netaddrequal(&a, &b)), "");

    testPrint(verbose, "Comparison (diff types)");
    b.len = ETH_ADDR_LEN;
    a.type = NETADDR_IPv4;
    failif((TRUE == netaddrequal(&a, &b)), "");

    testPrint(verbose, "Comparison (equal MACs)");
    a.type = NETADDR_ETHERNET;
    failif((FALSE == netaddrequal(&a, &b)), "");

    testPrint(verbose, "Comparison (diff MACs)");
    b.addr[5] = 0x00;
    failif((TRUE == netaddrequal(&a, &b)), "");

    /* Setup structures */
    a.type = NETADDR_IPv4;
    a.len = IPv4_ADDR_LEN;
    a.addr[0] = 192;
    a.addr[1] = 168;
    a.addr[2] = 1;
    a.addr[3] = 1;

    b.type = NETADDR_IPv4;
    b.len = IPv4_ADDR_LEN;
    b.addr[0] = 192;
    b.addr[1] = 168;
    b.addr[2] = 1;
    b.addr[3] = 1;

    testPrint(verbose, "Comparison (equal IPs)");
    failif((FALSE == netaddrequal(&a, &b)), "");

    testPrint(verbose, "Comparison (diff IPs)");
    a.addr[3] = 2;
    failif((TRUE == netaddrequal(&a, &b)), "");

    /* always print out the overall tests status */
    if (passed)
    {
        testPass(TRUE, "");
    }
    else
    {
        testFail(TRUE, "");
    }

#else /* NETHER */
    testSkip(TRUE, "");
#endif /* NETHER == 0 */
    return OK;
}
