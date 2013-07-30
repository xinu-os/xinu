/**
 * @file     xsh_memdump.c
 * @provides xsh_memdump.
 *
 * $Id: xsh_memdump.c 2065 2009-09-04 21:44:36Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <debug.h>
#include <mips.h>
#include <ctype.h>

#define DEC 10
#define HEX 16

static ulong parse(char *);
static ulong parseBase(char *, ushort);
static int digit(char);

/**
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
    if (nargs == 2 && strncmp(args[1], "--help", 7) == 0)
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
        if (0 == strncmp(args[arg], "-C", 2))
        {
            canon = TRUE;
        }
        else if (0 == strncmp(args[arg], "-f", 2))
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

    start = parse(args[arg + 0]);
    length = parse(args[arg + 1]);

    if (0 == start || 0 == length)
    {
        fprintf(stderr, "%s: invalid argument\n", args[0]);
        fprintf(stderr, "Try '%s --help' for more information\n",
                args[0]);
        return 1;
    }

    /* make sure the memdump values are somewhat sane */
    if ((TRUE == force) || (KSEG0_BASE <= start && start < KSEG2_BASE))
    {
        hexdump((uchar *)start, length, canon);
    }
    else
    {
        printf("Your values seem insane. Use -f to force.\n");
    }

    return 0;
}

static ulong parse(char *string)
{
    switch (string[0])
    {
    case '0':
        /* hex */
        if ('x' == string[1])
        {
            return parseBase(string + 2, HEX);
        }
        break;
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        /* dec */
        return parseBase(string, DEC);
        break;
    default:
        break;
    }

    /* error */
    return 0;
}

static ulong parseBase(char *string, ushort base)
{
    ushort pos;
    long sum;
    char c;
    long d;

    sum = 0;
    pos = 0;
    while ((c = string[pos]) != '\0')
    {
        if ((d = digit(c)) == SYSERR)
        {
            return 0;
        }

        if (base == DEC && isdigit(c))
        {
            sum = sum * DEC + d;
        }
        else if (base == HEX && isxdigit(c))
        {
            sum = sum * HEX + d;
        }
        else
        {
            return 0;
        }
        pos++;
    }

    return sum;
}

static int digit(char c)
{
    switch (c)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        return c - '0';
        break;
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
        return c - 'A' + 10;
        break;
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
        return c - 'a' + 10;
        break;
    }

    return -1;
}
