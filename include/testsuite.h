/**
 * @file testsuite.h
 * @provides failif.
 * Definitions relating to the Xinu testsuite.
 *
 * $Id: testsuite.h 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _TESTSUITE_H_
#define _TESTSUITE_H_

thread test_bigargs(bool);
thread test_schedule(bool);
thread test_preempt(bool);
thread test_recursion(bool);
thread test_semaphore(bool);
thread test_semaphore2(bool);
thread test_semaphore3(bool);
thread test_semaphore4(bool);
thread test_procQueue(bool);
thread test_deltaQueue(bool);
thread test_libStdio(bool);
thread test_libCtype(bool);
thread test_libString(bool);
thread test_libStdlib(bool);
thread test_libLimits(bool);
thread test_ttydriver(bool);
thread test_ether(bool);
thread test_ethloop(bool);
thread test_memory(bool);
thread test_bufpool(bool);
thread test_nvram(bool);
thread test_libQueue(bool);
thread test_system(bool);
thread test_mailbox(bool);
thread test_messagePass(bool);
thread test_netaddr(bool);
thread test_netif(bool);
thread test_arp(bool);
thread test_snoop(bool);
thread test_udp(bool);
thread test_raw(bool);
thread test_ip(bool);
thread test_umemory(bool);
thread test_tlb(bool);

void testPass(bool, const char *);
void testFail(bool, const char *);
void testSkip(bool, const char *);
void testPrint(bool, const char *);

/**
 * Causes the test to fail if condition is met and display failmsg in that
 * case.  Otherwise, the test will pass.
 * @param cond    condition for failure.
 * @param failmsg message to display if case fails.
 */
#define failif(cond, failmsg) \
	if ( cond ) { testFail(verbose, failmsg); passed = FALSE; } \
	else { testPass(verbose, ""); }

/**
 * Defines what a test case table entry looks like
 */
struct testcase
{
    char *name;                 /**< Name of test case                  */
    thread (*test) (bool);      /**< Test case function                 */
};

extern int ntests;                /**< total number of tests            */
extern struct testcase testtab[]; /**< table of test cases              */

#endif                          /* _TESTSUITE_H_ */
