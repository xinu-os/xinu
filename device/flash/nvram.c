/**
 * @file nvram.c
 * Functions to access the nvram settings from kernel space.
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <kernel.h>
#include <memory.h>
#include <device.h>
#include <nvram.h>
#include <string.h>
#include <stdlib.h>
#include <debug.h>
#include <stdio.h>

struct nvram_header *nvram_header = NULL;
struct nvram_tuple *nvram_tuples[NVRAM_NHASH];

static ulong nvramHash(char *name);
static void nvramInsert(struct nvram_tuple *tuple);
static uchar nvramCrc(uchar *header);

/**
 * Initialize the nvram variable structures for editing
 * @return OK on success, SYSERR on failure
 */
devcall nvramInit(void)
{
    struct dentry *devptr = NULL;
    struct flash *flash = NULL;
    uint nvbase, offset, index, pair_len;
    uint nvram_length, size;
    char *pair;
    struct nvram_tuple *tuple;

    /* check if we already have initialized nvram */
    if (nvram_header != NULL && NVRAM_MAGIC == nvram_header->magic)
    {
        return OK;
    }

#if FLASH
    devptr = (device *)&devtab[FLASH];
#endif
    if (NULL == devptr)
    {
        return SYSERR;
    }

    flash = &flashtab[devptr->minor];

    /* zero out nvram_tuples pointers */
    for (index = 0; index < NVRAM_NHASH; index++)
    {
        nvram_tuples[index] = NULL;
    }

    /* Scan flash at for NVRAM magic number */
    nvbase = flash->base + flash->size - NVRAM_SIZE;
    while (nvbase > flash->base)
    {
        if (NVRAM_MAGIC == *((uint *)nvbase))
            break;
        nvbase -= NVRAM_SIZE;
    }
    offset = 0;

    /* find the head for data */
    nvram_header =
        (struct nvram_header *)memget(sizeof(struct nvram_header));
    memcpy(nvram_header, (void *)nvbase, sizeof(struct nvram_header));
    if (nvram_header->magic != NVRAM_MAGIC)
    {
        return SYSERR;
    }

    offset += sizeof(struct nvram_header);

    /* loop through nvram variables and add to array */
    nvram_length = nvram_header->length;

    nvram_header->length = sizeof(struct nvram_header);

    while (offset < nvram_length)
    {
        /* get the length of a string (name=value\0) */
        pair = (char *)(nvbase + offset);
        pair_len = strnlen(pair, nvram_length - offset);

        /* set offset to next string */
        offset += pair_len + 1;

        if (pair_len <= 0)
        {
            continue;
        }

        /* allocate memory to store tuple */
        size = sizeof(struct nvram_tuple) + pair_len;
        tuple = memget(size);

        /* store tuple */
        memcpy(tuple->pair, pair, pair_len);
        memcpy(tuple->pair + pair_len, "\0", 1);


        nvramInsert(tuple);
    }

    return OK;
}

/**
 * Insert a tuple into the nvram_tuples table
 * @param *tuple pointer to tuple to add (should be memory address)
 * @return OK on success
 */
static void nvramInsert(struct nvram_tuple *tuple)
{
    uint index;
    struct nvram_tuple *curr;

    /* hash into table */
    index = nvramHash(tuple->pair);
    curr = nvram_tuples[index];

    /* search for an open position */
    while (curr != NULL && curr->next != NULL)
    {
        curr = curr->next;
    }

    /* fill the open position */
    if (NULL == curr)
    {
        curr = tuple;
        curr->next = NULL;
        nvram_tuples[index] = curr;
    }
    else
    {
        curr->next = tuple;
        tuple->next = NULL;
    }

    nvram_header->length += (strnlen(tuple->pair, NVRAM_STRMAX) + 1);
}

/**
 * Find the value of a variable.
 * @param *name name of variable to find
 * @return pointer to requested tuple struct
 */
char *nvramGet(char *name)
{
    struct nvram_tuple *tuple;
    uint hash;

    if (OK != nvramInit() || NULL == name)
    {
        return NULL;
    }

    /* hash the name */
    hash = nvramHash(name);

    /* iterate until name == name */
    tuple = nvram_tuples[hash];
    while ((tuple != NULL) &&
           (strncmp(name, tuple->pair, strnlen(name, NVRAM_STRMAX)) != 0))
    {
        tuple = tuple->next;
    }

    /* make sure a valid string was found, if not return NULL */
    if (NULL == tuple)
    {
        return NULL;
    }

    /* return pointer to start of value name=value pair */
    return tuple->pair + strnlen(name, NVRAM_STRMAX) + 1;
}

/**
 * Change the variable name to value. If name does not exist, create it,
 * otherwise replace the old value with the new.
 * @param *name  name to modify
 * @param *value value to set name
 * @return OK on successful change, SYSERR on failure
 */
devcall nvramSet(char *name, char *value)
{
    struct nvram_tuple *tuple;
    ulong length, offset;

    if (OK != nvramInit())
    {
        return SYSERR;
    }

    length = sizeof(struct nvram_tuple) + strnlen(name, NVRAM_STRMAX)
        + 1 + strnlen(value, NVRAM_STRMAX);

    /* check if name is already defined somewhere */
    /* if so unset, so we can reset */
    if (nvramGet(name) != NULL)
    {
        nvramUnset(name);
    }

    /* get enough space for both to be rounded up with \0 stored */
    if (NULL == (tuple = (struct nvram_tuple *)memget(length)))
    {
        return SYSERR;
    }

    /* copy name into memory */
    offset = 0;
    strncpy(tuple->pair, name, strnlen(name, NVRAM_STRMAX));
    offset += strnlen(name, NVRAM_STRMAX);
    strncpy(tuple->pair + offset, "=", 1);
    offset += 1;
    strncpy(tuple->pair + offset, value, strnlen(value, NVRAM_STRMAX));
    offset += strnlen(value, NVRAM_STRMAX);
    strncpy(tuple->pair + offset, "\0", 1);

    /* store pointer to name in nvram_tuple struct */
    nvramInsert(tuple);

    return OK;
}

/**
 * Remove a setting from nvram.
 * @param *name name to remove from nvram settings
 * @return OK on success, SYSERR on failure
 */
devcall nvramUnset(char *name)
{
    struct nvram_tuple *tuple, *prev;
    long index;
    uint length;

    if (OK != nvramInit())
    {
        return SYSERR;
    }

    /* find the pointer to the pair */
    if (SYSERR == (index = nvramHash(name)))
    {
        return SYSERR;
    }

    tuple = prev = nvram_tuples[index];

    while (tuple != NULL &&
           strncmp(tuple->pair, name, strnlen(name, NVRAM_STRMAX)))
    {
        prev = tuple;
        tuple = tuple->next;
    }

    if (NULL == tuple)
    {
        return SYSERR;
    }

    if (nvram_tuples[index] == tuple)
    {
        nvram_tuples[index] = tuple->next;
    }
    else
    {
        prev->next = tuple->next;
    }

    /* decrease length of nvram variables in header */
    length = strnlen(tuple->pair, NVRAM_STRMAX) + 1;
    nvram_header->length -= length;

    /* free memory node */
    if (SYSERR == memfree((void *)tuple, length))
    {
        return SYSERR;
    }

    return OK;
}

/**
 * Commit nvram settings to memory. Due to the nature of nvram settings
 * writing to memory everytime would greatly reduce the life of flash, so
 * after making changes you must commit them to memory.
 * @return OK on success, SYSERR on failure
 */
devcall nvramCommit()
{
    struct dentry *devptr = NULL;
    struct flash *flash = NULL;
    struct nvram_tuple *tuple;
    struct flash_block block, test_block;
    uchar *buffer;
    ulong n, offset, pos;

    if (OK != nvramInit())
    {
        return SYSERR;
    }

#if FLASH
    devptr = (device *)&devtab[FLASH];
#endif
    if (NULL == devptr)
    {
        return SYSERR;
    }
    flash = &flashtab[devptr->minor];

    /* length is maintained length + 2 (null string buffers) */
    /* and + 3 & ~0x03 to force word alignment               */
    nvram_header->length = (nvram_header->length + 2 + 3) & ~0x03;

    /* prepare a buffer to store all settings in */
    buffer = (uchar *)memget(nvram_header->length);
    if (SYSERR == (int)buffer)
    {
        return SYSERR;
    }
    bzero(buffer, nvram_header->length);

    /* clear header crc */
    nvram_header->crc_ver_init &= ~NVRAM_CRC_MASK;

    /* copy header into block */
    memcpy(buffer, nvram_header, sizeof(struct nvram_header));
    offset = sizeof(struct nvram_header);

    /* start at offset in block and write byte for byte */
    for (n = 0; n < NVRAM_NHASH; n++)
    {
        tuple = nvram_tuples[n];
        if (NULL == tuple)
        {
            continue;
        }
        do
        {
            /* copy tuple into buffer */
            memcpy(buffer + offset, tuple->pair,
                   strnlen(tuple->pair, NVRAM_STRMAX));
            /* next position + 1 for null-terminiation */
            offset += strnlen(tuple->pair, NVRAM_STRMAX) + 1;
            /* optionally, free the tuple memory */
            //memfree((void *)tuple);
        }
        while ((tuple = tuple->next) != NULL);
    }

    /* calculate crc */
    nvram_header->crc_ver_init |= ((uchar)nvramCrc(buffer));

    /* write new crc into block */
    memcpy(buffer, nvram_header, sizeof(struct nvram_header));


    /* to maintain data integrity check if the last block is being used */
    block = logicalMap(flash, NVRAM_MIN_BLOCK);

    for (n = 0; n < MAX_LIVE_BLOCKS; n++)
    {
        pos = (flash->curr_block + n) % MAX_LIVE_BLOCKS;
        test_block = flash->erase_blocks[pos];
        if (test_block.start_pos == block.start_pos)
        {
            if (FLASH_BLOCK_FREE != block.state)
            {
                block = test_block;
                break;
            }
        }
    }

    if (MAX_LIVE_BLOCKS <= n)
    {
        /* it is not cached in main memory */
        /* evict a block */
        physicalWrite(flash, &(flash->erase_blocks[flash->curr_block]));
        block.state = FLASH_BLOCK_FREE;
        /* read in block with nvram settings */
        physicalRead(flash, &block);
    }

    /* write the settings */
    offset = block.size - NVRAM_SIZE;
    memcpy(block.buffer + offset, buffer, nvram_header->length);
    block.state = FLASH_BLOCK_DIRTY;
    /* force a sync */
#if FLASH
    control(FLASH, FLASH_SYNC, FLASH_BLOCK, (long)&block);
#endif
    /* finally, free the block and return */
    memfree((void *)buffer, nvram_header->length);

    return OK;
}

/**
 * Compares a crc8 over the header data using the polynomial:
 *   x^8 + x^7 + x^6 + x^4 + x^2 + 1
 * @param header header for nvram data
 * @return crc value for header
 */
static uchar nvramCrc(uchar *header)
{
    uchar crc;
    ulong offset;

    /* CRC uses last 11 bytes of header plus tuples in LE order */
    crc = NVRAM_CRC_INIT;
    for (offset = 9; offset < nvram_header->length; offset++)
    {
        crc = nvram_crc_tab[crc ^ header[offset ^ 0]];
    }

    return crc;
}

/**
 * Given an input string, this will calculate a simple hash between 0 and
 * NVRAM_NHASH-1 (inclusive).
 * @param *p null or '=' terminated string
 * @return value of hashed string
 */
static ulong nvramHash(char *p)
{
    ulong sum = 0;
    while (*p && *p != '=')
    {
        sum = 13 * sum + *p++;
    }
    return sum % NVRAM_NHASH;
}
