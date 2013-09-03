/**
 * @file bzero.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <stdlib.h>
#include <string.h>

/**
 * @ingroup libxc
 *
 * Zeroes a block of memory.
 *
 * @param s
 *      Pointer to the start of the block of memory to zero.
 * @param n
 *      Length of the block of memory, in bytes.
 */
void bzero(void *s, size_t n)
{
    memset(s, 0, n);
}
