/**
 * @file atol.c
 * @provides atol.
 *
 * $Id: atol.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

/**
 * Converts an ascii value to a long.
 * @param *p pointer to ascii string
 * @return long
 */
long atol(char *p)
{
    long n;
    int f;

    n = 0;
    f = 0;
    for (;; p++)
    {
        switch (*p)
        {
        case ' ':
        case '\t':
            continue;
        case '-':
            f++;
        case '+':
            p++;
        }
        break;
    }
    while (*p >= '0' && *p <= '9')
    {
        n = n * 10 + *p++ - '0';
    }
    return (f ? -n : n);
}
