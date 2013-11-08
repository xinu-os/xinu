/**
 * @file teeWrite.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <tee.h>
#include <interrupt.h>

/**
 * Write a buffer of data to a tee device --- that is, the data is written to
 * all sub-devices associated with the tee device (except those that have had
 * ::TEE_SUBDEV_FLAG_WRITE_DISABLED set).
 *
 * @param devptr
 *      Pointer to the device table entry for a tee device.
 * @param buf
 *      Pointer to the buffer of data to write.
 * @param len
 *      Number of bytes to write.
 *
 * @return
 *      By default, returns the greatest number of bytes successfully written to
 *      all sub-devices, which can be up to @p len; or ::SYSERR if ::SYSERR was
 *      returned while writing to any subdevice.  If ::TEE_FLAG_BEST_WRITE is
 *      set on the tee device, then the return value is instead the greatest
 *      number of bytes successfully written to any sub-device, or ::SYSERR if
 *      ::SYSERR was returned when writing to all subdevices.
 */
devcall teeWrite(device *devptr, const void *buf, uint len)
{
    irqmask im;
    struct tee *tee;
    uint i;
    int retval = SYSERR;

    im = disable();
    tee = &teetab[devptr->minor];

    if (!tee->opened)
    {
        restore(im);
        return SYSERR;
    }

    for (i = 0; i < tee->num_subdevs; i++)
    {
        if (tee->subdev_flags[i] & TEE_SUBDEV_FLAG_WRITE_DISABLED)
        {
            continue;
        }

        int result = write(tee->subdevs[i], buf, len);

        if (tee->flags & TEE_FLAG_BEST_WRITE)
        {
            if (result > retval)
            {
                retval = result;
            }
        }
        else
        {
            if (SYSERR == result)
            {
                restore(im);
                return SYSERR;
            }
            retval = len = result;
        }
    }

    restore(im);
    return retval;
}
