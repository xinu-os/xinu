/**
 * @file     validateTrx.c
 * @provides validateTrx
 *
 * $Id: validateTrx.c 2065 2009-09-04 21:44:36Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <flash.h>

static void init_crc32(void);
static uint crc32buf(char *, uint);

/**
 * Validation for a TRX file type.
 * @param f the file to be validated
 * @return TRUE if file is a TRX, otherwise FALSE
 */
bool validateTrx(struct file *f)
{
    uint magic;
    uint len;
    uint crc;
    uint flags_vers;
    uint calc_crc;

    struct file *checkme;
    struct trx_header *header;


    bool retVal;
    retVal = TRUE;
    checkme = f;

    /* Cast the beginning of the file as a TRX header */
    header = (struct trx_header *)checkme->data;

    /* Initialize variables */
    magic = header->magic;
    len = header->len;
    crc = header->crc;
    flags_vers = header->flags_vers;

    /* Validate magic number */
    if (magic != TRX_MAGIC)
    {
        fprintf(stderr, "ERR: TRX header not found.\n");
        fprintf(stderr, "\tmagic found: %08X\t magic expected: %08X\n",
                magic, TRX_MAGIC);
        retVal = FALSE;
    }

    /* Validate file len is more than trx header */
    if (len < sizeof(struct trx_header))
    {
        fprintf(stderr, "ERR: TRX file size is too small\n");
        fprintf(stderr,
                "\tFile size is smaller than TRX header size (28 bytes)\n");
        retVal = FALSE;
    }

    /* Validate file length */
    if (len != checkme->len)
    {
        fprintf(stderr,
                "ERR: Expected and actual file length do not match\n");
        fprintf(stderr,
                "\tlen found: %d\tlen expected: %d\n", len, checkme->len);
        retVal = FALSE;
    }

    /* Validate checksum */
    if (retVal)
    {
        init_crc32();
        calc_crc = crc32buf((char *)(&header->flags_vers),
                            len - 3 * sizeof(int));
        if (crc != calc_crc)
        {
            fprintf(stderr, "ERR: Bad trx checksum\n");
            fprintf(stderr, "\tfound: %08X\texpected: %08X\n",
                    calc_crc, crc);
            retVal = FALSE;
        }
    }

    return retVal;
}

static unsigned long *crc32 = NULL;

static void init_crc32()
{
    unsigned long crc;
    unsigned long poly = 0xEDB88320L;
    int n, bit;

    if (crc32)
        return;

    crc32 = (unsigned long *)malloc(256 * sizeof(unsigned long));

    if (NULL == crc32)
    {
        return;
    }

    for (n = 0; n < 256; n++)
    {
        crc = (unsigned long)n;
        for (bit = 0; bit < 8; bit++)
        {
            crc = (crc & 1) ? (poly ^ (crc >> 1)) : (crc >> 1);
        }
        crc32[n] = crc;
    }
}

static unsigned int crc32buf(char *buf, uint len)
{
    unsigned int crc = 0xFFFFFFFF;

    if (!crc32)
    {
        return crc;
    }

    for (; len; len--, buf++)
    {
        crc = crc32[(crc ^ *buf) & 0xff] ^ (crc >> 8);
    }
    return crc;
}
