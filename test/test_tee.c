#include <testsuite.h>
#include <tee.h>
#include <device.h>
#include <stdio.h>
#include <loopback.h>
#include <string.h>
#include <stdlib.h>

#define DATASIZE 4

static int cmp_chars(const void *, const void *);

/**
 * Test tee driver (device/tee/)
 */
thread test_tee(bool verbose)
{
#if defined(TEE1) && defined(LOOP1)
    int ret;
    bool passed = TRUE;
    const uchar data[DATASIZE] = {'A', 'B', 'C', 'D'};
    const uchar data2[DATASIZE] = {'E', 'F', 'G', 'H'};
    /* data + data2 must be sorted --- see call to qsort() later  */
    uchar read_data[DATASIZE * 2 + 1];

    testPrint(verbose, "Opening LOOP0");
    ret = open(LOOP0);
    failif(OK != ret, "Can't open LOOP0");

    testPrint(verbose, "Opening LOOP1");
    ret = open(LOOP1);
    failif(OK != ret, "Can't open LOOP1");

    testPrint(verbose, "Setting LOOP0 to non-blocking mode");
    ret = control(LOOP0, LOOP_CTRL_SET_FLAG, LOOP_NONBLOCK, 0);
    failif(ret == SYSERR, "failed to set loopback device to nonblocking");

    testPrint(verbose, "Setting LOOP1 to non-blocking mode");
    ret = control(LOOP1, LOOP_CTRL_SET_FLAG, LOOP_NONBLOCK, 0);
    failif(ret == SYSERR, "failed to set loopback device to nonblocking");

    testPrint(verbose, "Opening TEE1 with subdevices LOOP0 and LOOP1");
    ret = open(TEE1, 2, LOOP0, LOOP1);
    failif(OK != ret, "");

    testPrint(verbose, "Closing TEE1");
    ret = close(TEE1);
    failif(OK != ret, "");

    testPrint(verbose, "Re-opening TEE1 with subdevices LOOP0 and LOOP1");
    ret = open(TEE1, 2, LOOP0, LOOP1);
    failif(OK != ret, "");

    testPrint(verbose, "Writing data to TEE1");
    ret = write(TEE1, data, DATASIZE);
    failif(ret != DATASIZE, "");

    testPrint(verbose, "Reading back data from LOOP0");
    memset(read_data, 0, sizeof(read_data));
    ret = read(LOOP0, read_data, DATASIZE + 1);
    failif(ret != DATASIZE || 0 != memcmp(data, read_data, DATASIZE), "");

    testPrint(verbose, "Reading back data from LOOP1");
    memset(read_data, 0, sizeof(read_data));
    ret = read(LOOP1, read_data, DATASIZE + 1);
    failif(ret != DATASIZE || 0 != memcmp(data, read_data, DATASIZE), "");

    testPrint(verbose, "Writing to LOOP0");
    ret = write(LOOP0, data, DATASIZE);
    failif(ret != DATASIZE, "");

    testPrint(verbose, "Writing to LOOP1");
    ret = write(LOOP1, data2, DATASIZE);
    failif(ret != DATASIZE, "");

    testPrint(verbose, "Reading back data from TEE1");
    memset(read_data, 0, sizeof(read_data));
    ret = read(TEE1, read_data, DATASIZE * 2 + 1);
    /* Data from two devices may be mixed up.  */
    qsort(read_data, DATASIZE * 2, 1, cmp_chars);
    failif(ret != DATASIZE * 2 ||
           0 != memcmp(read_data, data, DATASIZE) ||
           0 != memcmp(read_data + 4, data2, DATASIZE), "");

    testPrint(verbose, "Closing TEE1");
    ret = close(TEE1);
    failif(ret != OK, "");
    close(LOOP0);
    close(LOOP1);

    if (passed)
    {
        testPass(TRUE, "");
    }
    else
    {
        testFail(TRUE, "");
    }
#else
    testSkip(TRUE, "");
#endif
    return OK;
}

static int cmp_chars(const void *p1, const void *p2)
{
    return *(const uchar*)p1 - *(const uchar*)p2;
}
