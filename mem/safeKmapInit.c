/**
 * @file  safeKmapInit.c
 * @provides safeKmapInit.
 * Initialize kernel mappings.
 *
 * $Id: safeKmapInit.c 2020 2009-08-13 17:50:08Z mschul $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <kernel.h>

#include <memory.h>
#include <safemem.h>
#include <platform.h>
#include <stdlib.h>
#include <mips.h>

extern void _start(void);       /* start of Xinu code */

/**
 * Initialize kernel mappings to include statically configured kernel
 * memory.  Includes kernel text, data, stack, memory region table, and 
 * page table table.
 */
void safeKmapInit(void)
{
    void *start_addr;
    uint length;
    short attributes;

    /* setup pre-kernel space pages */
    start_addr = (void *)KSEG0_BASE;
    length = 0x1000;
    attributes = ENT_PRESENT | ENT_GLOBAL;
    safeMapRange(start_addr, length, attributes);

    /* setup kernel space pages */
    start_addr = (void *)_start;
    length = (uint)&_end - (uint)_start;
    attributes = ENT_PRESENT | ENT_GLOBAL;
    safeMapRange(start_addr, length, attributes);

    /* setup kernel stack space pages */
    start_addr = (void *)&_end;
    length = (uint)memheap - (uint)&_end;
    attributes = ENT_PRESENT | ENT_GLOBAL;
    safeMapRange(start_addr, length, attributes);

    /* setup kernel heap space pages */
    start_addr = (void *)memheap;
    length = (uint)memheap - (uint)&_end;
    attributes = ENT_PRESENT | ENT_GLOBAL;
    safeMapRange(start_addr, length, attributes);

    /* setup system page table mapping for KSEG2 */
    start_addr = (void *)pgtbl;
    length = pgtbl_nents * sizeof(struct pgtblent);
    attributes = ENT_PRESENT | ENT_GLOBAL;
    safeUnmapRange(start_addr, length); /* first unmap it */
    safeMapRange(start_addr, length, attributes);       /* now special map it */
}
