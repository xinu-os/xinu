/**
 * @file tftpGetIntoBuffer.c
 */
/* Embedded Xinu, Copyright (C) 2013.  All rights reserved. */

#include <tftp.h>
#include <memory.h>
#include <string.h>

static int tftpCopyIntoBufferCb(const uchar *data, uint len, void *ctx);

#define TFTP_FILE_DATA_BLOCK_SIZE 4096

struct tftpFileDataBlock
{
    ulong bytes_filled;
    struct tftpFileDataBlock *next;
    uchar data[TFTP_FILE_DATA_BLOCK_SIZE - sizeof(ulong) - sizeof(void*)];
};

/**
 * @ingroup tftp
 *
 * Download a file from a remote server using TFTP and allocates and return an
 * in-memory buffer containing the file contents.
 *
 * @param[in] filename
 *      Name of the file to download.
 * @param[in] local_ip
 *      Local protocol address to use for the connection.
 * @param[in] server_ip
 *      Remote protocol address to use for the connection (address of TFTP
 *      server).
 * @param[out] len_ret
 *      On success, the length of the file, which is also the length of the
 *      returned buffer, is written into this location.
 *
 * @return
 *      On success, returns a pointer (cast to an @c int) to a buffer containing
 *      the contents of the downloaded file.  This buffer is allocated with
 *      memget() and therefore can be freed using memfree().  On out-of-memory,
 *      timeout, file not found, or other error, returns ::SYSERR.
 */
syscall tftpGetIntoBuffer(const char *filename, const struct netaddr *local_ip,
                          const struct netaddr *server_ip, uint *len_ret)
{
    /* Unfortunately, TFTP (without extensions) provides no way to get the final
     * size of the resulting file.  Therefore, we allocate space block-by-block
     * and link them into a linked list, then copy the data into a single buffer
     * at the end.  Note: the sizes of the memory blocks stored in the linked
     * list (TFTP_FILE_DATA_BLOCK_SIZE) need not correspond to the TFTP block
     * size (which is, without extensions, always 512 bytes).  */

    struct tftpFileDataBlock *head, *ptr, *next, *tail;
    int retval;
    uchar *finalbuf;
    uint totallen;

    /* If the caller didn't ask for a place to store the length, then it would
     * be impossible for them to free the returned memory with memfree().  */
    if (NULL == len_ret)
    {
        TFTP_TRACE("Length pointer not supplied.");
        return SYSERR;
    }

    /* Allocate the head of the block list.  */
    head = memget(TFTP_FILE_DATA_BLOCK_SIZE);
    if (SYSERR == (int)head)
    {
        TFTP_TRACE("Out of memory.");
        return SYSERR;
    }
    head->bytes_filled = 0;
    head->next = NULL;
    tail = head;

    /* Download the file.  The callback function tftpCopyIntoBufferCb() is
     * responsible for building the block list with the received data.  */
    retval = tftpGet(filename, local_ip, server_ip, tftpCopyIntoBufferCb, &tail);

    /* Check return status.  */

    TFTP_TRACE("tftpGet() returned %d", retval);

    if (OK == retval)
    {
        /* Successfully downloaded the file.  Calculate the total length of the
         * file, then allocate the resulting buffer.  */
        totallen = 0;
        ptr = head;
        do
        {
            totallen += ptr->bytes_filled;
            ptr = ptr->next;
        } while (NULL != ptr);

        TFTP_TRACE("Allocating buffer for file data (%u bytes).", totallen);
        finalbuf = memget(totallen);
        if (SYSERR == (int)finalbuf)
        {
            TFTP_TRACE("Out of memory.");
        }
    }
    else
    {
        /* Failed to download the file.  Return SYSERR, but first the block list
         * needs to be freed.  */
        TFTP_TRACE("File download failed.");
        finalbuf = (uchar*)SYSERR;
    }

    /* Free the block list, and if the download was successful at the same time
     * copy the file data into the final buffer.  */
    TFTP_TRACE("Freeing block list and copying data into final buffer.");
    totallen = 0;
    next = head;
    do
    {
        ptr = next;
        next = ptr->next;
        if (SYSERR != (int)finalbuf)
        {
            memcpy(&finalbuf[totallen], ptr->data, ptr->bytes_filled);
        }
        totallen += ptr->bytes_filled;
        memfree(ptr, TFTP_FILE_DATA_BLOCK_SIZE);
    } while (NULL != next);

    /* If successful, save the file length in the caller-provided location.  */
    if (SYSERR != (int)finalbuf)
    {
        *len_ret = totallen;
        TFTP_TRACE("TFTP download into buffer successful "
                   "(address=0x%08x, length=%u)", finalbuf, totallen);
    }

    /* Return either the buffer containing the file data, or SYSERR.  */
    return (int)finalbuf;
}

/*
 * Callback function given to tftpGet() that is passed blocks of TFTP data.
 * This implementation stores the TFTP data in memory (block list is described
 * earlier in this file).
 *
 * This is expected to return OK on success, or SYSERR otherwise.
 */
static int tftpCopyIntoBufferCb(const uchar *data, uint len, void *ctx)
{
    struct tftpFileDataBlock **tailptr = ctx;
    struct tftpFileDataBlock *tail = *tailptr;

    while (0 != len)
    {
        struct tftpFileDataBlock *newtail;
        uint copylen;

        if (tail->bytes_filled == sizeof(tail->data))
        {
            /* Tail block is full; append a new block.  */
            newtail = memget(TFTP_FILE_DATA_BLOCK_SIZE);
            if (SYSERR == (int)newtail)
            {
                TFTP_TRACE("Out of memory.");
                return SYSERR;
            }
            newtail->bytes_filled = 0;
            newtail->next = NULL;
            tail->next = newtail;
            *tailptr = tail = newtail;
        }

        /* Store as much data as possible.  */
        if (len > sizeof(tail->data) - tail->bytes_filled)
        {
            copylen = sizeof(tail->data) - tail->bytes_filled;
        }
        else
        {
            copylen = len;
        }
        memcpy(&tail->data[tail->bytes_filled], data, copylen);
        tail->bytes_filled += copylen;
        len -= copylen;
        data += copylen;
    }
    return OK;
}
