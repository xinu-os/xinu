/**
 * @file waitOnBit.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <kernel.h>
#include <ether.h>

/**
 * @ingroup etherspecific
 *
 * Wait on a bit in a device's control and status register.
 * @param reg   pointer to device status register
 * @param mask  bit to wait on
 * @param value bit value desired
 * @param tries number of retries before giving up
 * @return OK or SYSERR
 */
int waitOnBit(volatile uint *reg, uint mask, const int value, int tries)
{
    int i = 0;
    volatile int j = 0;

    for (i = 0; i < tries; i++)
    {
        /* break on the following conditions:
         * - if expected value is non-zero, check if the register & mask has
         *   a non-zero value
         * - if expected value is zero, check if the register & mask has
         *   a zero value
         */
        if (((0 != value) && (*reg & mask))
            || ((0 == value) && !(*reg & mask)))
        {
            break;
        }

        for (j = 100; j; j--)
        {                       /* spin wait */
        }
    }

    if (tries == i)
    {
#ifdef DETAIL
        kprintf("etherInit() TIMEOUT waiting for register ");
        kprintf("0x%08X bit 0x%08X to %s\r\n", reg,
                mask, (value ? "set" : "clear"));
#endif                          /* DETAIL */
        return SYSERR;
    }

    return OK;
}
