/**
 * @file     tar.c
 * @provides tarGetData.
 *
 * $Id: tar.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <tar.h>
#include <string.h>

int tarFilesize(char *octalsize);

/**
 * Get a listing of filenames stored in a tar file.
 * @param archive pointer to archive data
 * @param filelist pointer to string array for list of files
 * @param nentries number of entries in filelist array
 * @return number of file entries found
 */
int tarListFiles(struct tar *archive, char *filelist, int nentries)
{
    int pos, filesize;
    int entries;
    struct tar *file;

    pos = 0;
    entries = 0;

    /* loop until the end of archive or full filelist */
    while (entries < nentries)
    {
        file = (struct tar *)&(((char *)archive)[pos]);

        /* check if at the end of archive */
        if (0x00 == file->filename[0])
        {
            break;
        }

        /* copy filename into file list */
        strncpy(&filelist[entries * TAR_FILENAME_LEN],
                file->filename, TAR_FILENAME_LEN);
        entries++;

        /* determine where the next file is located */
        filesize = tarFilesize(file->filesize);

        pos += roundtar(sizeof(struct tar) + filesize);
    }

    return entries;
}

/**
 * Get a pointer to a specific file in a tar format file.
 * @param archive tar file to process
 * @param filename name of file
 * @return pointer to tar header of filename
 */
struct tar *tarGetFile(struct tar *archive, char *filename)
{
    int pos, filesize;
    int entries;
    struct tar *file;

    pos = 0;
    entries = 0;

    /* loop until the end of archive or full filelist */
    while (1)
    {
        file = (struct tar *)&(((char *)archive)[pos]);

        /* check if at the end of archive */
        if (0x00 == file->filename[0])
        {
            break;
        }

        /* copy filename into file list */
        if (0 == strncmp(filename, file->filename, TAR_FILENAME_LEN))
        {
            return file;
        }

        /* determine where the next file is located */
        filesize = tarFilesize(file->filesize);

        pos += roundtar(sizeof(struct tar) + filesize);
    }

    return (struct tar *)NULL;
}

/**
 * Given a pointer to the tar header of a file, get the size of the file.
 * @param file pointer to tar header of file
 * @return size of file
 */
int tarGetFilesize(struct tar *file)
{
    /* determine the file size (stored in octal string) */
    return tarFilesize(file->filesize);
}

/**
 * Given a pointer to the tar header of a file, fill buffer with data
 * stored in the file.
 * @param file pointer to tar header of file
 * @param buffer buffer to store data in
 * @param size size of buffer
 * @return number of bytes copied into buffer
 */
int tarGetData(struct tar *file, char *buffer, uint size)
{
    int filesize;
    char *data;

    /* point to data section of file */
    data = file->type.data;

    /* determine the file size (stored in octal string) */
    filesize = tarFilesize(file->filesize);

    /* is the file ustar format? */
    if (0 == strncmp((void *)&(file->type.ustar.isustar), "ustar", 5))
    {
        /* update data pointer and file size */
        data = file->type.ustar.data;
    }

    /* check bounds */
    if (size > filesize)
    {
        size = filesize;
    }

    /* copy data */
    memcpy(buffer, data, size);

    return size;
}

/**
 * Decode the filesize of a tar file.  Filesize is stored as an octal
 * string.
 * @param octalsize string holding size stored in octal
 * @return size of file
 */
int tarFilesize(char *octalsize)
{
    int i;
    int size;

    size = 0;
    for (i = 0; i < (TAR_FILESIZE_LEN - 1); i++)
    {
        size *= TAR_FILESIZE_BASE;
        size += (octalsize[i] - '0');
    }

    return size;
}
