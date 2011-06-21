/**
 * @file strncmp.c
 * @provides sstrncmp.
 *
 * $Id: strncmp.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

/**
 * Compare strings (at most n bytes).
 * @param *s1 first memory location
 * @param *s2 second memory location
 * @param n length to compare
 * @return s1>s2: >0  s1==s2: 0  s1<s2: <0
 */
int strncmp(char *s1, char *s2, int n)
{

    while (--n >= 0 && *s1 == *s2++)
    {
        if (*s1++ == '\0')
        {
            return 0;
        }
    }
    return (n < 0 ? 0 : *s1 - *--s2);
}
