/**
 * @file bzero.c
 * @provides bzero.
 *
 * $Id: bzero.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

/**
 * Clears a block of characters to 0s
 * @param *p pointer to start of block
 * @param len length of the block
 */
void bzero(void *p, int len)
{
    int n;
    char *pch = (char *)p;

    if ((n = len) <= 0)
    {
        return;
    }
    do
    {
        *pch++ = 0;
    }
    while (--n);
}
