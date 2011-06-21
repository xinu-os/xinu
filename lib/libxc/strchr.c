/**
 * @file strchr.c
 * @provides strchr.
 *
 * $Id: strchr.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

/** 
 * Returns a pointer to the location in a string at which which a particular
 * character appears.
 * @param *s string to search
 * @param c character to locate
 * @return the pointer in the string, NULL if character not found
 */
char *strchr(const char *s, int c)
{
    for (; *s != '\0'; s++)
    {
        if (*s == (const char)c)
        {
            return (char *)s;
        }
    }

    if ((const char)c == *s)
    {
        return (char *)s;
    }

    return 0;
}
