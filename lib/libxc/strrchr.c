/**
 * @file strrchr.c
 * @provides strrchr.
 *
 * $Id: strrchr.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

/**
 * Returns a pointer to the location in a string at which which a particular
 * character last appears.
 * @param *s string to search
 * @param c character to locate
 * @return the pointer in the string, NULL if character not found
 */
char *strrchr(const char *s, int c)
{
    char *r = 0;

    for (; *s != '\0'; s++)
    {
        if (*s == (const char)c)
        {
            r = (char *)s;
        }
    }

    if ((const char)c == *s)
    {
        return (char *)s;
    }

    return r;
}
