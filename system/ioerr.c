/**
 * @file ioerr.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>

/**
 * @ingroup devcalls
 *
 * Unconditionally return an error (used for "error" entries in devtab).
 *
 * @return ::SYSERR
 */
devcall ioerr(void)
{
    return SYSERR;
}
