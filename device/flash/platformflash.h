/**
 * @file platformflash.h
 * This file provides platform dependant flash constants
 *
 */
/* Embedded XINU, Copyright (C) 2009.  All rights reserved. */

#ifndef _PLATFORMFLASH_H_
#define _PLATFORMFLASH_H_

/* commands for data gathering */
#ifdef CFI_ASUS
#define CFI_QUERY_EXIT    0xF0
#else
#define CFI_QUERY_EXIT    0xFF
#endif

#endif                          /* _FLASH_H_ */
