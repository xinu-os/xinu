#include <stddef.h>
#include <stdio.h>
#include <testsuite.h>

/**< table of test cases */
struct testcase testtab[] = {
    {"Argument Passing", test_bigargs},
    {"Priority Scheduling", test_schedule},
    {"Thread Preemption", test_preempt},
    {"Recursion", test_recursion},
    {"Single Semaphore", test_semaphore},
    {"Multiple Semaphores", test_semaphore2},
    {"Counting Semaphores", test_semaphore3},
    {"Killing Semaphores", test_semaphore4},
    {"Process Queues", test_procQueue},
    {"Delta Queues", test_deltaQueue},
    {"Standard Input/Output", test_libStdio},
    {"TTY Driver", test_ttydriver},
    {"Character Types", test_libCtype},
    {"String Library", test_libString},
    {"Standard Library", test_libStdlib},
    {"Type Limits", test_libLimits},
    {"Memory", test_memory},
    {"Buffer Pool", test_bufpool},
    {"NVRAM", test_nvram},
    {"System", test_system},
    {"Message Passing", test_messagePass},
    {"Mailbox", test_mailbox},
    {"Ethernet Driver", test_ether},
    {"Ethernet Loopback Driver", test_ethloop},
    {"Tee driver", test_tee},
    {"Network Addresses", test_netaddr},
    {"Network Interface", test_netif},
    {"ARP", test_arp},
    {"Snoop", test_snoop},
    {"UDP Sockets", test_udp},
    {"Raw Sockets", test_raw},
    {"IP", test_ip},
    {"User Memory", test_umemory},
    {"Simple TLB", test_tlb},
};

int ntests = sizeof(testtab) / sizeof(struct testcase);

void testPass(bool verbose, const char *msg)
{
    if (TRUE == verbose)
    {
        printf("\033[60G[\033[1;32mPASS\033[0;39m] %s\n", msg);
    }
}

void testFail(bool verbose, const char *msg)
{
    if (TRUE == verbose)
    {
        printf("\033[60G[\033[1;31mFAIL\033[0;39m] %s\n", msg);
    }
}

void testSkip(bool verbose, const char *msg)
{
    if (TRUE == verbose)
    {
        printf("\033[60G[\033[1;33mSKIP\033[0;39m] %s\n", msg);
    }
}

void testPrint(bool verbose, const char *msg)
{
    if (TRUE == verbose)
    {
        printf("  %s", msg);
    }
}
