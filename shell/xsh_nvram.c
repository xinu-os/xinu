/**
 * @file     xsh_nvram.c
 * Shell function wrapper to lower level nvram settings.
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <kernel.h>
#include <nvram.h>
#include <stdio.h>
#include <string.h>

#if NVRAM
static void xsh_nvramHelp(char *command);
static void xsh_nvramList(void);
static int xsh_nvramSet(char *pair);

/**
 * @ingroup shell
 *
 * Command 'nvram', allows shell access to nvram variables
 * @param nargs number of arguments
 * @param args  array of arguments
 * @return non-zero vaule on error
 */
shellcmd xsh_nvram(int nargs, char *args[])
{
    char *value;

    /* make sure nvram is initialized */
    if (SYSERR == nvramInit())
    {
        fprintf(stderr, "error: device does not appear to have nvram.\n");
    }

    if (2 == nargs && 0 == strcmp(args[1], "list"))
    {
        xsh_nvramList();
        return 0;
    }
    else if (3 == nargs && 0 == strcmp(args[1], "get"))
    {
        value = nvramGet(args[2]);
        if (value)
        {
            printf("%s\n", nvramGet(args[2]));
            return 0;
        }
        else
        {
            fprintf(stderr, "no binding\n");
            return 1;
        }
    }
    else if (3 == nargs && 0 == strcmp(args[1], "set"))
    {
        return xsh_nvramSet(args[2]);
    }
    else if (3 == nargs && 0 == strcmp(args[1], "unset"))
    {
        return nvramUnset(args[2]);
    }
    else if (2 == nargs && 0 == strcmp(args[1], "commit"))
    {
        return nvramCommit();
    }


    xsh_nvramHelp(args[0]);

    return 1;
}

/**
 * Display help information about nvram command
 */
static void xsh_nvramHelp(char *command)
{
    printf("Usage: %s [--help] [get <NAME> | ", command);
    printf("set <NAME>=<VALUE> | unset <NAME> | list]\n\n");
    printf("Description:\n");
    printf("\tProvides shell access to nvram variables\n");
    printf("Options:\n");
    printf("\t--help\t\t\tdisplay this help and exit\n");
    printf("\tlist\t\t\tshow a listing of all <NAME>=<VALUE> tuples\n");
    printf
        ("\tget <NAME>\t\tdisplay tuple <NAME>=<VALUE>, if it exists\n");
    printf("\tset <NAME>=<VALUE>\tset variable <NAME> to <VALUE>,\n");
    printf("\t\t\t\tcreate if non-existent\n");
    printf("\tunset <NAME>\t\tremove variable name from nvram\n");
    printf("\tcommit\t\t\tcommits the current nvram settings to flash\n");
}

/**
 * Provide a listing of all the nvram variables known on Flash
 */
static void xsh_nvramList(void)
{
    ushort n, count;
    struct nvram_tuple *tuple;

    count = 0;
    for (n = 0; n < NVRAM_NHASH; n++)
    {
        tuple = nvram_tuples[n];
        if (NULL == tuple)
        {
            continue;
        }
        do
        {
            printf("%s\n", tuple->pair);
            count++;
        }
        while ((tuple = tuple->next) != NULL);
    }

    printf("%d pairs using %d bytes (%d bytes remaining)\n",
           count, nvram_header->length,
           NVRAM_SIZE - nvram_header->length);
}

/**
 * Change the value of nvram variable name to value
 * @param *pair name=value pair
 * @return OK (creates a variable if non-existent)
 */
static int xsh_nvramSet(char *pair)
{
    char *name, *value;

    name = pair;
    while (*pair != '\0' && *pair++ != '=');

    if (*pair == '\0')
    {
        return SYSERR;
    }

    *(pair - 1) = '\0';
    value = pair;

    return nvramSet(name, value);
}
#endif /* NVRAM */
