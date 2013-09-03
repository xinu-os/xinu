/**
 * @file ionull.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>

/**
 * @ingroup devcalls
 *
 * Do nothing (use for irrelevant entries in devtab).
 *
 * @return ::OK
 */
devcall ionull(void)
{
    return OK;
}
