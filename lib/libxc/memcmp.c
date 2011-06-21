/**
 * @file memcmp.c
 * @provides memcmp.
 *
 * $Id: memcmp.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

/**
 * Compare memory (ISO C89).
 * Assumes memory locations are same length
 * @param s1 first memory location
 * @param s2 second memory location
 * @param n length to compare
 * @return s1>s2: >0  s1==s2: 0  s1<s2: <0
 */
int memcmp(const void *s1, const void *s2, int n)
{
    const unsigned char *c1;
    const unsigned char *c2;

    for (c1 = s1, c2 = s2; n > 0; n--, c1++, c2++)
    {
        if (*c1 != *c2)
        {
            return ((int)*c1) - ((int)*c2);
        }
    }
    return 0;
}
