/**
 * @file usbKbdRead.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <usbkbd.h>
#include <interrupt.h>

/**
 * Read data from a USB keyboard.
 *
 * @param devptr
 *      Pointer to the device table entry for a USB keyboard.
 * @param buf
 *      Pointer to a buffer into which to place the read data.
 * @param len
 *      Maximum number of bytes of data to read.
 *
 * @return
 *      On success, returns the number of bytes read, which normally is @p len,
 *      but may be less than @p len if the keyboard has been set to non-blocking
 *      mode.  Returns ::SYSERR on other error (currently, only if usbKbdInit()
 *      has not yet been called).
 */
devcall usbKbdRead(device *devptr, void *buf, uint len)
{
    irqmask im;
    struct usbkbd *kbd;
    uint count;

    /* Disable interrupts and get a pointer to the keyboard structure.  */
    im = disable();
    kbd = &usbkbds[devptr->minor];

    /* Make sure usbKbdInit() has run.  */
    if (!kbd->initialized)
    {
        restore(im);
        return SYSERR;
    }

    /* Attempt to read each byte requested.  */
    USBKBD_TRACE("Attempting to read %u bytes from keyboard", len);
    for (count = 0; count < len; count++)
    {
        /* If the keyboard is in non-blocking mode, ensure there is a byte
         * available in the input buffer from the interrupt handler.  If not,
         * return early with a short count.  */
        if ((kbd->iflags & USBKBD_IFLAG_NOBLOCK) && kbd->icount == 0)
        {
            break;
        }

        /* Wait for there to be at least one byte in the input buffer from the
         * interrupt handler, then remove it.  */
        wait(kbd->isema);
        ((uchar*)buf)[count] = kbd->in[kbd->istart];
        kbd->icount--;
        kbd->istart = (kbd->istart + 1) % USBKBD_IBLEN;
    }

    /* Restore interrupts and return the number of bytes read.  */
    restore(im);
    USBKBD_TRACE("Return count=%u", count);
    return count;
}
