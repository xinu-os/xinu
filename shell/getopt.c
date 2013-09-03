/**
 * @file     getopt.c
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <string.h>
#include <shell.h>

/**
 * @ingroup shell
 *
 * Xinu approximation of the POSIX getopt function for parsing command line
 * args.
 * Note: *opts.optreset should be set to TRUE on first call.
 *
 * @param argc count of arguments
 * @param **argv vector of argument strings
 * @param *optstring string of options to look for
 * @param *opts `struct getopt` to keep track of progress
 * @return -1 when no more args are found, option character otherwise
 */
int getopt(int argc, char **argv, char *optstring, struct getopt *opts)
{
    char *currarg;
    char *optpos;

    /* reset to defaults */
    if (TRUE == opts->optreset)
    {
        opts->argc = argc;
        opts->argv = argv;
        opts->optstring = optstring;
        opts->optarg = "";
        opts->optind = 1;
        opts->optopt = 0;
        opts->opterr = 0;
        opts->optreset = FALSE;
    }

    /* make sure we have something to parse */
    if (opts->argc <= opts->optind)
    {
        return -1;
    }

    /* check if the current arg is an option */
    currarg = opts->argv[opts->optind];
    if ('-' == currarg[0])
    {
        opts->optind++;
        /* strchr doesn't have a strnchr equiv. in Xinu right now... */
        optpos = strchr(opts->optstring, currarg[1]);
        if (NULL == optpos)
        {
            /* no match */
            return '?';
        }
        else
        {
            /* match, is there a value after */
            if (':' == optpos[1] && opts->argc > opts->optind)
            {
                opts->optarg = opts->argv[opts->optind];
                opts->optind++;
            }
            else
            {
                /* missing required argument */
                return ':';
            }
            return currarg[1];
        }
    }
    else
    {
        /* not an argument, assume parsing is done */
        return -1;
    }
}
