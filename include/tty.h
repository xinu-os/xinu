/**
 * @file tty.h
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _TTY_H_
#define _TTY_H_

#include <device.h>
#include <stdarg.h>
#include <stddef.h>

#ifndef TTY_IBLEN
#define TTY_IBLEN           1024 /**< input buffer length               */
#endif

/* TTY input flags */
#define TTY_IRAW            0x01 /**< read unbuffered and uncooked      */
#define TTY_INLCR           0x02 /**< convert '\n' to '\r'              */
#define TTY_IGNCR           0x04 /**< ignore '\r' on input              */
#define TTY_ICRNL           0x08 /**< convert '\r' to '\n'              */
#define TTY_ECHO            0x10 /**< echo input                        */
#define TTY_IALL (TTY_IRAW | TTY_INLCR | TTY_IGNCR | TTY_ICRNL | TTY_ECHO)

/* TTY output flags */
#define TTY_ONLCR           0x01 /**< convert '\n' to '\r\n'            */
#define TTY_OCRNL           0x02 /**< convert '\r' to '\n'              */

/* ttyControl() functions  */
#define TTY_CTRL_SET_IFLAG  0x20 /**< set input flags                   */
#define TTY_CTRL_CLR_IFLAG  0x21 /**< clear input flags                 */
#define TTY_CTRL_SET_OFLAG  0x22 /**< set output flags                  */
#define TTY_CTRL_CLR_OFLAG  0x23 /**< clear output flags                */

/**
 * TTY control block
 */
struct tty
{
    /* Pointers to associated structures */
    device *phw;                /**< hardware device structure          */

    /* TTY input fields */
    uchar iflags;               /**< Input flags, TTY_I* above          */
    bool ieof;                  /**< EOF read                           */
    bool idelim;                /**< partial line in buffer             */
    char in[TTY_IBLEN];         /**< input buffer                       */
    uint istart;                /**< index of first char in buffer      */
    uint icount;                /**< number of characters in buffer     */

    /* TTY output fields */
    uchar oflags;               /**< Output flags, TTY_O* above         */
};

extern struct tty ttytab[];

/* Driver functions */
devcall ttyInit(device *);
devcall ttyOpen(device *, va_list);
devcall ttyClose(device *);
devcall ttyRead(device *, void *, uint);
devcall ttyWrite(device *, void *, uint);
devcall ttyGetc(device *);
devcall ttyPutc(device *, char);
devcall ttyControl(device *, int, long, long);

#endif                          /* _TTY_H_ */
