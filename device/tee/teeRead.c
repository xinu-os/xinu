/**
 * @file teeRead.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <tee.h>
#include <stdio.h>

static thread teeReader(struct tee *, uint);

/**
 * Reads data from a tee device --- that is, the characters are taken from any
 * of the subdevices associated with the tee device (except those that have had
 * ::TEE_SUBDEV_FLAG_READ_DISABLED set), and characters read from different
 * subdevices may be intermixed.
 *
 * @param devptr
 *      Pointer to the device table entry for a tee device.
 * @param buf
 *      Pointer to a buffer into which to place the read data.
 * @param len
 *      Maximum number of bytes of data to read.
 *
 * @return
 *      If @len was greater than 0 and no bytes were successfully read, returns
 *      ::SYSERR.  Otherwise returns the number of bytes successfully read,
 *      which can be up to @len.  Reading is stopped early only if errors have
 *      occurred on all subdevices.
 */
devcall teeRead(device *devptr, void *buf, uint len)
{
    struct tee *tee;
    uint i;
    uint count;
    irqmask im;

    im = disable();
    tee = &teetab[devptr->minor];

    if (!tee->opened)
    {
        restore(im);
        return SYSERR;
    }

    if (!tee->readers_started)
    {
        tee->isema = semcreate(0);
        if (SYSERR == tee->isema)
        {
            restore(im);
            return SYSERR;
        }

        for (i = 0; i < tee->num_subdevs; i++)
        {
            if (tee->subdev_flags[i] & TEE_SUBDEV_FLAG_READ_DISABLED)
            {
                tee->reader_tids[i] = BADTID;
                continue;
            }

            char name[32];
            sprintf(name, "teeReader%d_%u", devptr->minor, i);

            tee->reader_tids[i] = create(teeReader,
                                         TEE_READER_THRSTK, TEE_READER_PRIO,
                                         name, 2, tee, i);
            if (SYSERR == ready(tee->reader_tids[i], RESCHED_NO))
            {
                uint j;
                for (j = 0; j < i; j++)
                {
                    kill(tee->reader_tids[j]);
                }
                semfree(tee->isema);
                restore(im);
                return SYSERR;
            }
        }
        tee->readers_started = TRUE;
    }

    for (i = 0; i < tee->num_subdevs; i++)
    {
        tee->subdev_flags[i] &= ~TEE_SUBDEV_FLAG_ERROR;
    }

    /* Attempt to read each byte requested.  */
    for (count = 0; count < len; )
    {
        /* Wait for there to be at least one byte in the input buffer from any
         * subdevice, or for an error to occur.  */
        wait(tee->isema);

        if (0 != tee->icount)
        {
            /* At least one byte is available.  */
            uchar c = tee->in[tee->istart];
            ((uchar*)buf)[count] = c;
            tee->icount--;
            tee->istart = (tee->istart + 1) % TEE_IBLEN;
            count++;
        }
        else
        {
            /* No bytes available --- must have been an error.  */
            for (i = 0; i < tee->num_subdevs; i++)
            {
                if (!(tee->subdev_flags[i] & TEE_SUBDEV_FLAG_ERROR))
                {
                    break;
                }
            }
            if (i == tee->num_subdevs)
            {
                /* Error occurred on all subdevices --- stop early  */
                if (count == 0)
                {
                    count = SYSERR;
                }
                break;
            }
        }
    }

    /* Restore interrupts and return the number of bytes read.  */
    restore(im);
    return count;
}

/* Procedure for thread to read data from tee subdevice and feed it into the tee
 * device's input buffer  */
static thread teeReader(struct tee *tee, uint i)
{
    for (;;)
    {
        uchar c;
        int retval;
        irqmask im;

        retval = read(tee->subdevs[i], &c, 1);
        im = disable();
        if (retval == 1)
        {
            /* Successfully read a byte  */
            tee->subdev_flags[i] &= ~TEE_SUBDEV_FLAG_ERROR;
            if (tee->icount < TEE_IBLEN)
            {
                tee->in[(tee->istart + tee->icount) % TEE_IBLEN] = c;
                tee->icount++;
                signal(tee->isema);
            }
            else
            {
                /* XXX: Overrun  */
            }
        }
        else
        {
            /* Read error or end-of-file  */
            tee->subdev_flags[i] |= TEE_SUBDEV_FLAG_ERROR;
            if (semcount(tee->isema) <= 0)
            {
                signal(tee->isema);
            }
            sleep(TEE_ERROR_WAIT);
        }
        restore(im);
    }
    return SYSERR;
}
