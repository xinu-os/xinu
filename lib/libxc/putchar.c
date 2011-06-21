/**
 * @file putchar.c
 * @provides putchar.
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stdio.h>

int putchar(int c)
{
    return fputc(c, stdout);
}
