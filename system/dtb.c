/**
 * @file dtb.c This file provides support for parsing in-memory Flat
 * Devicetree data structure.  For more information see the Devicetree
 * Specification at https://www.devicetree.org/
 */

/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#include <stddef.h>
#include <device.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <endianness.h>

#define FDT_MAGIC  0xd00dfeed
#define FDT_LAST_COMP_VERSION  2

/* print the tree as it's being parsed */
#define FDT_DETAILS 0

#if FDT_DETAILS
#define dtb_debug(format, ...) kprintf (format, ##__VA_ARGS__)
#else
#define dtb_debug(format, ...) 
#endif

struct fdt_header {
    uint32_t magic;
    uint32_t totalsize;
    uint32_t off_dt_struct;
    uint32_t off_dt_strings;
    uint32_t off_mem_rsvmap;
    uint32_t version;
    uint32_t last_comp_version;
    uint32_t boot_cpuid_phys; 
    uint32_t size_dt_strings;
    uint32_t size_dt_struct;
}; 

struct fdt_reserve_entry {
    uint64_t address;
    uint64_t size; 
}; 

void process_rsvmap(struct fdt_reserve_entry* eptr,
		    void (*rsv_handler)(uint64_t address,
					uint64_t size)) {
    while ( eptr->address || eptr->size) {
	dtb_debug("     protected region at %lX of size %lX",
		  be64_to_cpu(eptr->address),
		  be64_to_cpu(eptr->size));
	if (NULL != rsv_handler) {
	    rsv_handler(be64_to_cpu(eptr->address),
			be64_to_cpu(eptr->address));
	}
	eptr++;
    }
}

#define FDT_BEGIN_NODE 0x1
#define FDT_END_NODE   0x2
#define FDT_PROP       0x3
#define FDT_NOP        0x4
#define FDT_END        0x9




struct fdt_prop {
    uint32_t len;
    uint32_t nameoff;
};

uint32_t c2ioff(uint32_t charoff) {
    return (charoff / 4) + ((charoff % 4) ? 1 : 0);
}

#if FDT_DETAILS
/* known properties that we know how to interpret */
static char* fdt_string_prop_names[] = {"bootargs",
					"compatible",
					"device_type",
					"stdout-path",
					"stdin-path",
					"model",
					"status",
					"name",
					"mmu-type",
					"riscv,isa",
					NULL};
static char* fdt_u32_prop_names[] = {"reg",
				     "timebase-frequency",
				     "phandle",
				     "interrupts",
				     "interrupt-parent",
				     "#address-cells",
				     "#size-cell",
				     "clock-frequency",
				     NULL};


int fdt_prop_in_list(char* prop, char**lst) {
    int i;
    for(i = 0; NULL!= *(lst+i); i++) {
	if (!strcmp(prop, *(lst+i))) {
	    return 1;
	}
    }
    return 0;
}

void fdt_print_prop(char* node_name,
		    char *prop_name,
		    unsigned char *prop_val,
		    uint32_t prop_len) {
    dtb_debug("     node=%s prop_name='%s' len=%u%s",
	    node_name, prop_name, prop_len, (prop_len ? " value=" : ""));
    // how do we know data type? only by name?
    int j;
    if (fdt_prop_in_list(prop_name, fdt_string_prop_names)) {
	dtb_debug("'%s'", prop_val);
    } else if (fdt_prop_in_list(prop_name, fdt_u32_prop_names)) {
	dtb_debug("<");
	for (j = 0 ; j < prop_len; j+=4) {
	    dtb_debug("%s0x%x",
		    (j ? " " : ""),
		    be32_to_cpu(*((uint32_t*)(prop_val + j))));
	}
	dtb_debug(">");
    } else {
	for (j = 0 ; j < prop_len; j++) {
	    dtb_debug("%s%02x", (j ? " " : ""), *(prop_val + j));
	}
    }
    dtb_debug("\n");
}
#endif

int process_fdt_nodes(void* fdt_strb_ptr, uint32_t *startptr,
		      void (*handler)(char* nname,
				      char *pname,
				      unsigned char *val,
				      uint32_t len)) {
    uint32_t i = 0;
    char *node_name = NULL;
    while (1) {
	//dtb_debug("  process_fdt_nodes: offset=%0u\n", i);
	uint32_t*tokptr = startptr + i;
	//dtb_debug("  process_fdt_nodes: tokptr 0x%lX\n", tokptr);
	uint32_t token = be32_to_cpu(*tokptr);
	//dtb_debug("  process_fdt_nodes: token=0x%X\n", token);
	switch(token) {
	case FDT_BEGIN_NODE:
	    // FDT_BEGIN_NODE followed by 0 term. name
	    node_name = (char*)(tokptr+1);
	    dtb_debug("  begin node at %lX: '%s'\n", tokptr, node_name);
	    i += 1 + c2ioff(1 + strlen((char*)(tokptr+1)));
	    continue;
	case FDT_PROP:
	    ; /* ; keeps compiler happy, var decls aren't statements
		 and can't be labeled */
	    struct fdt_prop *propptr = (struct fdt_prop*)(tokptr+1);
	    uint32_t prop_len = be32_to_cpu(propptr->len);
	    char *prop_name = (char*)(fdt_strb_ptr + be32_to_cpu(propptr->nameoff));
	    unsigned char *prop_val = ((unsigned char*)propptr) + sizeof(struct fdt_prop);
#if FDT_DETAILS
	    fdt_print_prop(node_name, prop_name, prop_val, prop_len);
#endif	    
	    if (NULL != handler) {
		handler(node_name, prop_name, prop_val, prop_len);
	    }
	    i+= 1 + c2ioff(sizeof(struct fdt_prop) + prop_len);
	    
	    continue;
	case FDT_END_NODE:
	    node_name = NULL;
	case FDT_NOP:
	    i++;
	    continue;
	case FDT_END:
	    dtb_debug("  FDT end at %lX\n", tokptr);
	    return 0;
	default:
	    dtb_debug("  unknown token '%x' at %lX\n", token, tokptr+i);
	    return 1;
	}
    
    }
    return 0;
}


int parse_dtb(void *dtbptr,
	      void (*prop_handler)(char *nname,
				   char *pname,
				   unsigned char *val,
				   uint32_t len),
	      void (*rsv_handler)(uint64_t address,
				  uint64_t size)) {
    struct fdt_header* h = (struct fdt_header*)dtbptr;
    dtb_debug("* Dump of dtb at 0x%0lX\n", dtbptr);
    dtb_debug("  magic: 0x%x\n", be32_to_cpu(h->magic));
    if (FDT_MAGIC!=be32_to_cpu(h->magic)) {
	dtb_debug("Error: magic is invalid: got 0x%X, but 0x%X was expected\n",
		h->magic, FDT_MAGIC);
	return SYSERR;
    }
    dtb_debug("  version: %u\n", be32_to_cpu(h->version));
    int lcompver = be32_to_cpu(h->last_comp_version);
    dtb_debug("  last compatible version: %u\n", lcompver);
    if (lcompver < FDT_LAST_COMP_VERSION) {
	dtb_debug("WARNING: unexpected last_comp_version: got %u, but only %u is supported, parsing mail fail!",
		lcompver, FDT_LAST_COMP_VERSION);
    }
    dtb_debug("  memory reservations map is at offset: 0x%x\n", be32_to_cpu(h->off_mem_rsvmap));
    process_rsvmap((struct fdt_reserve_entry*)(dtbptr + be32_to_cpu(h->off_mem_rsvmap)),
		   rsv_handler);
    dtb_debug("  structure block is at offset: 0x%x\n", be32_to_cpu(h->off_dt_struct));
    dtb_debug("  strings block is at offset: 0x%x\n", be32_to_cpu(h->off_dt_strings));
    process_fdt_nodes((void*) (dtbptr + be32_to_cpu(h->off_dt_strings)),
		      (uint32_t*)(dtbptr+ be32_to_cpu(h->off_dt_struct)),
		      prop_handler);
    return 0;
}

uint64_t dtb_read_uint64(uint32_t* dataptr) {
    return ((((uint64_t)be32_to_cpu(*dataptr))<<32) | ((uint64_t)be32_to_cpu(*(dataptr+1))));
}




