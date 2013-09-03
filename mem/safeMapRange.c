/**
 * @file safeMapRange.c
 * Map a range of pages to a page table.
 *
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <safemem.h>

/**
 * Map a range of pages to a page table starting at start, ending at
 * start+length.
 * @param start beginning page address to map.
 * @param length length of memory range to map.
 * @param attr attributes to apply to page table entries.
 * @return non-zero value on failure.
 */
int safeMapRange(void *start, uint length, short attr)
{
    uint addr, end;
    int result;

    length = roundpage(length);
    addr = (uint)truncpage(start);
    end = addr + length;

    for (; addr < end; addr += PAGE_SIZE)
    {
        if ((result = safeMap((void *)addr, attr)) != 0)
        {
            return result;
        }
    }

    return 0;
}
