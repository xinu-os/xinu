/**
 * @file ioerr.c
 * @provides ioerr.
 *
 * $Id: ioerr.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>

/**
 * return an error (used for "error" entries in devtab)
 * @return SYSERR
 */
devcall ioerr(void)
{
    return SYSERR;
}
