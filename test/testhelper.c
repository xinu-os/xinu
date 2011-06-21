#include <stddef.h>
#include <stdio.h>
#include <testsuite.h>

/**< table of test cases */
struct testcase testtab[] = {
    {"Argument Passing", test_bigargs},
    {"Priority Scheduling", test_schedule},
    {"Thread Preemption", test_preempt},
    {"Recursion", test_recursion},
#if NSEM
    {"Single Semaphore", test_semaphore},
    {"Multiple Semaphores", test_semaphore2},
    {"Counting Semaphores", test_semaphore3},
    {"Killing Semaphores", test_semaphore4},
#endif
    {"Process Queues", test_procQueue},
    {"Delta Queues", test_deltaQueue},
#if LOOP
    {"Standard Input/Output", test_libStdio},
    {"TTY Driver", test_ttydriver},
#endif
    {"Character Types", test_libCtype},
    {"String Library", test_libString},
    {"Standard Library", test_libStdlib},
    {"Type Limits", test_libLimits},
    {"Memory", test_memory},
    {"Buffer Pool", test_bufpool},
#if NVRAM
    {"NVRAM", test_nvram},
#endif
    {"System", test_system},
    {"Message Passing", test_messagePass},
#if NMAILBOX
    {"Mailbox", test_mailbox},
#endif
#if NETHER
    {"Ethernet Driver", test_ether},
    {"Ethernet Loopback Driver", test_ethloop},
    {"Network Addresses", test_netaddr},
    {"Network Interface", test_netif},
    {"ARP", test_arp},
    {"Snoop", test_snoop},
    {"UDP Sockets", test_udp},
    {"Raw Sockets", test_raw},
    {"IP", test_ip},
#endif
    {"User Memory", test_umemory},
#if USE_TLB
    {"Simple TLB", test_tlb},
#endif                          /* USE_TLB */
};

int ntests = sizeof(testtab) / sizeof(struct testcase);

void testPass(bool verbose, const char *msg)
{
    if (TRUE == verbose)
    {
        printf("\033[40G[\033[1;32mPASS\033[0;39m] %s\n", msg);
    }
}

void testFail(bool verbose, const char *msg)
{
    if (TRUE == verbose)
    {
        printf("\033[40G[\033[1;31mFAIL\033[0;39m] %s\n", msg);
    }
}

void testSkip(bool verbose, const char *msg)
{
    if (TRUE == verbose)
    {
        printf("\033[40G[\033[1;33mSKIP\033[0;39m] %s\n", msg);
    }
}

void testPrint(bool verbose, const char *msg)
{
    if (TRUE == verbose)
    {
        printf("  %s", msg);
    }
}
