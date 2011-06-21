/**
 * @file strncpy.c
 * @provides strncpy.
 *
 * $Id: strncpy.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

/**
 * Copy string s2 to s1, truncating or null-padding to always copy n bytes.
 * @param s1 first string
 * @param s2 second string
 * @param n length of s2 to copy
 * @return s1
 */
char *strncpy(char *s1, const char *s2, int n)
{
    register int i;
    register char *os1;

    os1 = s1;
    for (i = 0; i < n; i++)
    {
        if (((*s1++) = (*s2++)) == '\0')
        {
            while (++i < n)
            {
                *s1++ = '\0';
            }
            return os1;
        }
    }
    return os1;
}
