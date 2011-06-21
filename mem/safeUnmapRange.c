/**
 * @file  safeUnmapRange.c
 * @provides safeUnmapRange.
 * Remove a range of pages from page table.
 *
 * $Id: safeUnmapRange.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <safemem.h>

/**
 * Remove a range of pages from a page table starting at start and ending
 * after length bytes.
 * @param start beginning page address to remove.
 * @param length number of bytes to unmap for.
 * @return non-zero value on failure.
 */
int safeUnmapRange(void *start, uint length)
{
    uint addr, end;
    int result;

    length = roundpage(length);
    addr = (uint)truncpage(start);
    end = addr + length;

    for (; addr < end; addr += PAGE_SIZE)
    {
        if ((result = safeUnmap((void *)addr)) != 0)
        {
            return result;
        }
    }

    return 0;
}
