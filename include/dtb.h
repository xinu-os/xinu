/**
 * @file dhcpc.h
 *
 * Devicetree parser interface
 */
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#ifndef _DTB_H_
#define _DTB_H_


/* parse dtb tree:
   Arguments: pointer to the in-memory flat device tree
              pointer to callback that will handle parsed properties
	      pointer to callback that will handle reserved memory blocks
*/              
int parse_dtb(void *dtbptr,
	      void (*prop_handler)(char *node_name,
				   char *property_name,
				   unsigned char *val,
				   uint32_t len),
	      void (*rsv_handler)(uint64_t address,
				  uint64_t size));
uint64_t dtb_read_uint64(uint32_t* dataptr);

#endif
