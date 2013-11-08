/**
 * @file teeControl.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <tee.h>
#include <interrupt.h>

static int teeModifySubdevFlag(struct tee *tee, int func, int subdev, uchar flags);

/**
 * Control the operation of a tee device.
 */
devcall teeControl(device *devptr, int func, long arg1, long arg2)
{
    irqmask im;
    struct tee *tee;
    int retval;

    im = disable();
    tee = &teetab[devptr->minor];
    retval = SYSERR;

    if (!tee->opened)
    {
        restore(im);
        return retval;
    }

    switch (func)
    {
        case TEE_CTRL_SUBDEV_SET_FLAGS:
        case TEE_CTRL_SUBDEV_CLEAR_FLAGS:
        case TEE_CTRL_SUBDEV_GET_FLAGS:
            retval = teeModifySubdevFlag(tee, func, arg1, arg2);
            break;

        case TEE_CTRL_SET_FLAGS:
            retval = tee->flags & arg1;
            tee->flags |= arg1;
            break;

        case TEE_CTRL_CLEAR_FLAGS:
            retval = tee->flags & arg1;
            tee->flags &= ~arg1;
            break;

        case TEE_CTRL_GET_FLAGS:
            retval = tee->flags;
            break;
    }

    restore(im);
    return retval;
}

/* Set or clear a flag on a tee sub-device.  Returns 1 if flag was previously
 * set and 0 if not.  */
static int teeModifySubdevFlag(struct tee *tee, int func, int subdev,
                               uchar flags)
{
    uint i;
    int retval = SYSERR;

    /* Find index of this sub-device within the tee device  */
    for (i = 0; i < tee->num_subdevs; i++)
    {
        if (tee->subdevs[i] == subdev)
        {
            break;
        }
    }

    if (i == tee->num_subdevs)
    {
        return retval;
    }

    switch (func)
    {
    case TEE_CTRL_SUBDEV_SET_FLAGS:
        retval = tee->subdev_flags[i] & flags;
        tee->subdev_flags[i] |= flags;
        break;

    case TEE_CTRL_SUBDEV_CLEAR_FLAGS:
        retval = tee->subdev_flags[i] & flags;
        tee->subdev_flags[i] &= ~flags;
        break;

    case TEE_CTRL_SUBDEV_GET_FLAGS:
        retval = tee->subdev_flags[i];
        break;
    }
    return retval;
}
