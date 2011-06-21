/**
 * @file memset.c
 * @provides memset.
 *
 * $Id: memset.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

/** 
 * Place a character into first n characters.
 * @param s memory to place character into
 * @param c character to place
 * @param n number of times to place character
 * @return the pointer to memory
 */
void *memset(void *s, int c, int n)
{
    register int i;
    char *cp = (char *)s;

    for (i = 0; i < n; i++)
    {
        *cp = (unsigned char)c;
        cp++;
    }
    return s;
}
