/**
 * @file tar.h
 *
 * $Id: tar.h 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _TAR_H_
#define _TAR_H_

#include <stddef.h>

#define TAR_FILESIZE_LEN  12
#define TAR_FILESIZE_BASE 8
#define TAR_FILENAME_LEN  100

struct ustar
{
    char isustar[6];
    char version[2];
    char owner_name[32];
    char group_name[32];
    char dev_major[8];
    char dev_minor[8];
    char fileprefix[155];
    char buffer[12];
    char data[1];
};

struct tar
{
    char filename[TAR_FILENAME_LEN];
    char filemode[8];
    char owner_id[8];
    char group_id[8];
    char filesize[TAR_FILESIZE_LEN];
    char modtime[12];
    char checksum[8];
    char typeflag;
    char linkname[TAR_FILENAME_LEN];
    union
    {
        struct ustar ustar;
        char data[1];
    } type;
};

/* tar uses 512 byte blocks */
#define roundtar(size) ((511 + (uint)(size)) & ~0x1ff)

#define TAR_LINK_NORMAL '0'
#define TAR_LINK_HARD   '1'
#define TAR_LINK_SYMB   '2'
#define TAR_LINK_CHAR   '3'
#define TAR_LINK_BLOCK  '4'
#define TAR_LINK_DIR    '5'
#define TAR_LINK_FIFO   '6'

/* function prototypes */
int tarListFiles(struct tar *, char *, int);
struct tar *tarGetFile(struct tar *, char *);
int tarGetFilesize(struct tar *);
int tarGetData(struct tar *, char *, uint);

#endif                          /* _TAR_H_ */
