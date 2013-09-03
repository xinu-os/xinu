/**
 * @file usb_util.h
 * @ingroup usbcore
 *
 * Miscellaneous definitions used by the USB code.
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#ifndef _USB_UTIL_H_
#define _USB_UTIL_H_

#include <kernel.h>
#include <stddef.h>

/** Unsigned integer type of exactly 8 bits.  */
typedef uchar  u8;

/** Unsigned integer type of exactly 16 bits.  */
typedef ushort u16;

/** Unsigned integer type of exactly 32 bits.  */
typedef uint   u32;

/** Type-independent macro to calculate the minimum of 2 values.  */
#define min(a, b) ({ __typeof__(a) _a = (a); __typeof__(b) _b = (b); \
                            (_a < _b) ? _a : _b; })

/** Type-independent macro to calculate the maximum of 2 values.  */
#define max(a, b) ({ __typeof__(a) _a = (a); __typeof__(b) _b = (b); \
                            (_a > _b) ? _a : _b; })

/** Pack a structure with no padding.  */
#define __packed                    __attribute__((packed))

/** Align the member or buffer on a word boundary.  */
#define __word_aligned              __attribute__((aligned(sizeof(ulong))))

/** Provide compiler warnings like with printf().  */
#define __printf_format(fmt, vargs) __attribute__((format(printf, fmt, vargs)))

/** Round a number up to the next multiple of the word size.  */
#define WORD_ALIGN(n) (((n) + sizeof(ulong) - 1) & ~(sizeof(ulong) - 1))

/** Determines whether a pointer is word-aligned or not.  */
#define IS_WORD_ALIGNED(ptr) ((ulong)(ptr) % sizeof(ulong) == 0)

/** Perform integer division, rounding up the quotient.  */
#define DIV_ROUND_UP(num, denom) (((num) + (denom) - 1) / (denom))

/** Get the number of elements in an array (not dynamically allocated)  */
#define ARRAY_LEN(array) (sizeof(array) / sizeof((array)[0]))

/** Assert that something is true at compilation time.  This generates no code
 * in the resulting binary.  */
#define STATIC_ASSERT(condition) ((void)sizeof(char[1 - 2*!(condition)]))

static inline void
__usb_check_uint_types(void)
{
    STATIC_ASSERT(sizeof(u8)  == 1);
    STATIC_ASSERT(sizeof(u16) == 2);
    STATIC_ASSERT(sizeof(u32) == 4);
}

struct usb_device;

/**********************************************************************/

/* Configuration variables (could be moved somewhere else).  */

/** Enable USB "embedded" mode: if set to TRUE, there will be no USB debugging,
 * info, or error messages, and the 'usbinfo' shell command will not be
 * available.  This will reduce the compiled code size significantly.  */
#define USB_EMBEDDED                FALSE

/** Minimum priority for USB messages.  Only messages with priority greater than
 * or equal to this will be printed.  */
#define USB_MIN_LOG_PRIORITY        2

/**********************************************************************/

/** Priority for USB error messages.  */
#define USB_LOG_PRIORITY_ERROR      3

/** Priority for USB informational messages, such as a device being attached or
 * detached.  */
#define USB_LOG_PRIORITY_INFO       2

/** Priority for USB debugging messages.  */
#define USB_LOG_PRIORITY_DEBUG      1

#if USB_EMBEDDED && USB_MIN_LOG_PRIORITY <= USB_LOG_PRIORITY_ERROR
#  undef USB_MIN_LOG_PRIORITY
#  define USB_MIN_LOG_PRIORITY USB_LOG_PRIORITY_ERROR + 1
#endif

#if USB_LOG_PRIORITY_ERROR >= USB_MIN_LOG_PRIORITY
void usb_log(int priority, const char *func,
             struct usb_device *dev, const char *format, ...)
                __printf_format(4, 5);
#endif

#if USB_LOG_PRIORITY_ERROR >= USB_MIN_LOG_PRIORITY
#  define usb_dev_error(dev, format, ...) \
        usb_log(USB_LOG_PRIORITY_ERROR, __func__, dev, format, ##__VA_ARGS__)
#else
#  define usb_dev_error(dev, format, ...)
#endif

#if USB_LOG_PRIORITY_INFO >= USB_MIN_LOG_PRIORITY
#  define usb_dev_info(dev, format, ...) \
        usb_log(USB_LOG_PRIORITY_INFO, __func__, dev, format, ##__VA_ARGS__)
#else
#  define usb_dev_info(dev, format, ...)
#endif

#if USB_LOG_PRIORITY_DEBUG >= USB_MIN_LOG_PRIORITY
#  define usb_dev_debug(dev, format, ...) \
        usb_log(USB_LOG_PRIORITY_DEBUG, __func__, dev, format, ##__VA_ARGS__)
#else
#  define usb_dev_debug(dev, format, ...)
#endif

#define usb_error(format, ...) usb_dev_error(NULL, format, ##__VA_ARGS__)
#define usb_info(format, ...)  usb_dev_info (NULL, format, ##__VA_ARGS__)
#define usb_debug(format, ...) usb_dev_debug(NULL, format, ##__VA_ARGS__)

/** Status code returned by many functions in the USB subsystem.  The generic
 * Xinu SYSERR does not provide enough information in many cases.  */
typedef enum usb_status {

    /** Function successful.  */
    USB_STATUS_SUCCESS                   =  0,

    /** USB device was detached.  */
    USB_STATUS_DEVICE_DETACHED           = -1,

    /** USB device is unsupported by the driver.  */
    USB_STATUS_DEVICE_UNSUPPORTED        = -2,

    /** Hardware error of some form occurred.  */
    USB_STATUS_HARDWARE_ERROR            = -3,

    /** Invalid data was received.  */
    USB_STATUS_INVALID_DATA              = -4,

    /** An invalid parameter was passed to the function.  */
    USB_STATUS_INVALID_PARAMETER         = -5,

    /** The USB transfer has not yet been processed.  */
    USB_STATUS_NOT_PROCESSED             = -6,

    /** Failed to allocated needed memory.  */
    USB_STATUS_OUT_OF_MEMORY             = -7,

    /** The operation timed out.  */
    USB_STATUS_TIMEOUT                   = -8,

    /** The request is unsupported.  */
    USB_STATUS_UNSUPPORTED_REQUEST       = -9,
} usb_status_t;

#if !USB_EMBEDDED
const char *usb_status_string(usb_status_t status);
#endif

#endif /* _USB_UTIL_H_ */
