/**
 * @file     xsh_sctp.c
 * @provides xsh_sctp.
 *
 * $Id: xsh_sctp.c 2070 2009-09-18 22:36:02Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <sctp.h>
#include <network.h>

int sctp_stat(int argc, char **argv);
int sctp_test(int argc, char **argv);
int sctp_chat(int argc, char **argv);

static int usage(char *prog)
{
    fprintf(stderr, "usage: %s (stat|test|chat) [-h]\n", prog);
    return 0;
}

/**
 * Perform a variety of SCTP commands. `stat` for statistics, `test` for
 * testing, `chat` for sending/receiving message (like `sctp_darn`).
 * @param nargs number of arguments
 * @param args  array of arguments
 * @return non-zero value on error
 */
shellcmd xsh_sctp(int nargs, char *args[])
{
    printf("Got %d args\n", nargs);

    if (nargs < 2)
    {
        return usage(args[0]);
    }
    else if (strncmp("stat", args[1], 4) == 0)
    {
        return sctp_stat(nargs - 2, &args[2]);
    }
    else if (strncmp("test", args[1], 4) == 0)
    {
        return sctp_test(nargs - 2, &args[2]);
    }
    else if (strncmp("chat", args[1], 4) == 0)
    {
        return sctp_chat(nargs - 2, &args[2]);
    }
    else
    {
        return usage(args[0]);
    }
}

int sctp_stat(int argc, char **argv)
{
    printf("got stat(%d)\n", argc);
    return 0;
}

int sctp_test(int argc, char **argv)
{
    struct netaddr local;
    dot2ipv4("192.168.6.103", &local);

    sctpInitialize(53233, &local, 1);
    return 0;
}

int sctp_chat(int argc, char **argv)
{
    printf("got chat(%d)\n", argc);
    return 0;
}
