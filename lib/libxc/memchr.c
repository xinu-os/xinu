/**
 * @file memchr.c
 * @provides memchr.
 *
 * $Id: memchr.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

/** 
 * Returns a pointer to the location in  memory at which which a particular
 * character appears.
 * @param *cs string to search
 * @param c character to locate
 * @param n number of bytes to search
 * @return the pointer in the string, NULL if character not found
 */
void *memchr(const void *cs, int c, int n)
{
    char *cp = (char *)cs;

    for (; *cp != '\0'; cp++)
    {
        if (*cp == (unsigned char)c)
        {
            return (void *)cp;
        }
    }
    return 0;
}
