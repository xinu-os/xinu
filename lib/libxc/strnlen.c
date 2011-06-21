/**
 * @file strnlen.c
 * @provides strnlen.
 *
 * $Id: strnlen.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

/**
 * Returns the number of non-NULL bytes in a string or len.
 * @param *s string
 * @param len length you wish to scan s too
 * @return length of the string up to user specified length
 */
int strnlen(const char *s, unsigned int len)
{
    int n;

    n = 0;
    while (*s++ && n < len)
        n++;

    return (n);
}
