/**
 * @file     xsh_netemu.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdlib.h>
#include <date.h>

/**
 * @ingroup shell
 *
 * Shell command (netemu). Allows user to set network emulator options.
 * @param nargs number of arguments
 * @param args  array of arguments
 * @return non-zero value on error
 */
shellcmd xsh_netemu(int nargs, char *args[])
{
    srand(getRdate("192.168.6.10"));
    return 0;

}
