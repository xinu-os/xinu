/**
 * @file     xsh_memdump.c
 */
/* Embedded Xinu, Copyright (C) 2009, 2013.  All rights reserved. */

#include <ctype.h>
#include <debug.h>
#include <mips.h>
#include <platform.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

static ulong string_to_addr(const char *s);

#define INVALID_ADDR (~(ulong)0)

static bool address_range_sane(ulong start, ulong len)
{
    if (len == 0)
    {
        return TRUE;
    }
    if (start + len < start)
    {
        return FALSE;
    }
#ifdef _XINU_ARCH_MIPS_
    return (KSEG0_BASE <= start && start + len <= KSEG2_BASE);
#else
    return ((ulong)platform.minaddr <= start &&
            start + len <= (ulong)platform.maxaddr);
#endif
}

/**
 * @ingroup shell
 *
 * Dump a region of memory to stdout.
 * @param nargs number of arguments
 * @param args  array of arguments
 * @return non-zero value on error
 */
shellcmd xsh_memdump(int nargs, char *args[])
{
    bool canon = FALSE;
    bool force = FALSE;
    ulong start, length;
    ushort arg;

    /* Output help, if '--help' argument was supplied */
    if (nargs == 2 && strcmp(args[1], "--help") == 0)
    {
        printf("Usage: %s [-C] [-f] <START> <LENGTH>\n\n", args[0]);
        printf("Description:\n");
        printf("\tDumps the region of memory starting at START\n");
        printf("\tand completing at START+LENGTH to standard out\n");
        printf("\tSTART and LENGTH can be hex or dec.\n");
        printf("Options:\n");
        printf("\t<START>\t\tregion of memory at which to start\n");
        printf("\t<LENGTH>\tlength to be dumped\n");
        printf("\t-C\t\tdisplay in canonical mode (hex+ASCII)\n");
        printf("\t-f\t\tignore sanity checks for addresses\n");
        printf("\t--help\t\tdisplay this help and exit\n");
        return 1;
    }

    if (nargs < 3 || nargs > 5)
    {
        fprintf(stderr, "%s: too few arguments\n", args[0]);
        fprintf(stderr, "Try '%s --help' for more information\n",
                args[0]);
        return 1;
    }

    for (arg = 1; arg < nargs; arg++)
    {
        if (0 == strcmp(args[arg], "-C"))
        {
            canon = TRUE;
        }
        else if (0 == strcmp(args[arg], "-f"))
        {
            force = TRUE;
        }
        else
        {
            break;
        }
        nargs--;
    }

    if (nargs != 3)
    {
        fprintf(stderr, "%s: too few arguments\n", args[0]);
        fprintf(stderr, "Try '%s --help' for more information\n",
                args[0]);
        return 1;
    }

    start = string_to_addr(args[arg + 0]);
    length = string_to_addr(args[arg + 1]);

    if (INVALID_ADDR == start || INVALID_ADDR == length)
    {
        fprintf(stderr, "%s: invalid argument\n", args[0]);
        fprintf(stderr, "Try '%s --help' for more information\n",
                args[0]);
        return 1;
    }

    /* make sure the memdump values are somewhat sane */
    if ((TRUE == force) || address_range_sane(start, length))
    {
        hexdump((uchar *)start, length, canon);
    }
    else
    {
        printf("Your values seem insane. Use -f to force.\n");
    }

    return 0;
}

static ulong string_to_addr(const char *string)
{
    ulong n;
    const char *scan = string;
    const char *format = "%lu";
    if (string[0] == '0' && (string[1] == 'x' || string[1] == 'X'))
    {
        scan += 2;
        format = "%lx";
    }
    else if (string[0] == '0' && isdigit((uchar)string[1]))
    {
        format = "%lo";
    }
    if (sscanf(scan, format, &n) != 1)
    {
        n = INVALID_ADDR;
    }
    return n;
}
