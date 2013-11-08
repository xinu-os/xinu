/**
 * @file tee.h
 *
 * Header for the "tee" driver, which provides pseudo-devices that
 * can duplicate writes to multiple devices and/or combine reads
 * from multiple devices.
 */

#ifndef _TEE_H
#define _TEE_H

#include <conf.h>
#include <device.h>
#include <semaphore.h>
#include <stdarg.h>

/** Maximum number of subdevices per tee device  */
#define TEE_MAX_SUBDEVS 8

/** Maximum number of input bytes to buffer  */
#define TEE_IBLEN 1024

/** Stack size of threads spawned to read from sub-devices  */
#define TEE_READER_THRSTK 8192

/** Priority of threads spawned to read from sub-devices  */
#define TEE_READER_PRIO   50

/** Number of milliseconds to wait before retrying subdevice read after error  */
#define TEE_ERROR_WAIT    100

/** Tee device control block  */
struct tee
{
    bool opened;                      /**< Is this tee device open?           */
    bool readers_started;             /**< Have reader threads been started?  */
    uchar flags;                      /**< Tee device flags                   */
    uint num_subdevs;                 /**< Actual number of subdevices        */
    int subdevs[TEE_MAX_SUBDEVS];     /**< Device IDs of subdevices           */
    uchar subdev_flags[TEE_MAX_SUBDEVS]; /**< Flags for each subdevice        */
    tid_typ reader_tids[TEE_MAX_SUBDEVS]; /**< Subdevice reader thread IDs    */
    semaphore isema;                  /**< Input semaphore                    */
    ushort istart;                    /**< Index of first input byte          */
    ushort icount;                    /**< Number of input bytes              */
    uchar in[TEE_IBLEN];              /**< Input buffer                       */
};

extern struct tee teetab[];

/* tee device control flags  */

/** Affects teePutc() and teeWrite():  Return the return value from the most
 * successful subdevice write rather than the least.  */
#define TEE_FLAG_BEST_WRITE             0x01

/** Error occured on subdevice (do not modify via ioctl)  */
#define TEE_SUBDEV_FLAG_ERROR           0x01

/** Read disabled on this subdevice.  Note: modifying this flag currently only
 * has an effect before the first read occurs on the tee device.  */
#define TEE_SUBDEV_FLAG_READ_DISABLED   0x02

/** Write disabled on this subdevice  */
#define TEE_SUBDEV_FLAG_WRITE_DISABLED  0x04

/* tee device control requests  */
#define TEE_CTRL_SUBDEV_SET_FLAGS   0x01  /**< Set subdevice flags     */
#define TEE_CTRL_SUBDEV_CLEAR_FLAGS 0x02  /**< Clear subdevice flags   */
#define TEE_CTRL_SUBDEV_GET_FLAGS   0x03  /**< Get subdevice flags     */
#define TEE_CTRL_SET_FLAGS          0x11  /**< Set tee device flags    */
#define TEE_CTRL_CLEAR_FLAGS        0x12  /**< Clear tee device flags  */
#define TEE_CTRL_GET_FLAGS          0x13  /**< Get tee device flags    */

/* tee driver functions --- only call through device entries  */
devcall teeClose(device *devptr);
devcall teeControl(device *devptr, int func, long arg1, long arg2);
devcall teeGetc(device *devptr);
devcall teeOpen(device *devptr, va_list va);
devcall teePutc(device *devptr, char c);
devcall teeRead(device *devptr, void *buf, uint len);
devcall teeWrite(device *devptr, const void *buf, uint len);

/* tee driver utility functions  */
int teeAlloc(void);

#endif /* _TEE_H */
