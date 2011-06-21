/**
 * @file     xsh_tar.c
 * @provides xsh_tar.
 *
 * $Id: xsh_tar.c 2070 2009-09-18 22:36:02Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <tar.h>

extern int _binary_data_mytar_tar_start;

#define NFILES     5
#define BUFFERSIZE 8096

void tarHelp(char *command);

/**
 * Shell command demonstrating reading of tar files.
 * @param nargs number of arguments
 * @param args  array of arguments
 * @return non-zero value on error
 */
shellcmd xsh_tar(int nargs, char *args[])
{
    int i, size, totalsize;
    char filelist[NFILES][TAR_FILENAME_LEN];
    char data[BUFFERSIZE];
    struct tar *archive;
    struct tar *file;

#if USE_TAR
    archive = (struct tar *)&_binary_data_mytar_tar_start;
#else
    return SYSERR;
#endif

    if (nargs != 2)
    {
        tarHelp(args[0]);
        return 1;
    }

    if (0 == strncmp("--help", args[1], 7))
    {
        tarHelp(args[0]);
        return 0;
    }

    if (0 == strncmp("ls", args[1], 2))
    {
        size = tarListFiles(archive, (char *)filelist, NFILES);

        for (i = 0; i < size; i++)
        {
            printf("file: '%s'\n", filelist[i]);
        }

        return 0;
    }

    file = tarGetFile(archive, args[1]);

    if (file != NULL)
    {
        totalsize = tarGetFilesize(file);
        printf("File '%s' is %d bytes.\n", args[1], totalsize);
        bzero(data, BUFFERSIZE);
        tarGetData(file, data, BUFFERSIZE - 1);
        data[BUFFERSIZE - 1] = '\0';
        printf("%s", data);
    }
    else
    {
        fprintf(stderr, "File does not exist.\n");
        return 1;
    }

    return 0;
}

void tarHelp(char *command)
{
    printf("Usage: %s (ls|<FILENAME>)\n\n", command);
    printf("Description:\n");
    printf("\tDump contents of a tar file\n");
    printf("Options:\n");
    printf("\tls\t\tdisplay listing of files in tar file\n");
    printf("\t<FILENAME>\tfilename within tar file to display\n");
    printf("\t--help\t\tdisplay this help screen\n");
}
