/**
 * @file ionull.c
 * @provides ionull.
 *
 * $Id: ionull.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>

/**
 * do nothing (use for irrelevant entries in devtab)
 * @return OK
 */
devcall ionull(void)
{
    return OK;
}
