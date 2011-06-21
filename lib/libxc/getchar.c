/**
 * @file getchar.c
 * @provides getchar.
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stdio.h>

int getchar(void)
{
    return fgetc(stdin);
}
