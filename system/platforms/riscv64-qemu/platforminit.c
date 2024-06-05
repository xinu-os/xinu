/**
 * @file platforminit.c
 */
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */

#include <platform.h>
#include <string.h>
#include <stdint.h>
#include <kernel.h>
#include <endianness.h>
#include <riscv.h>
#include <dtb.h>

/**
 * RISCV QEMU hardware platform specific initialization 
 *
 * @return OK
 */

void *dtb_addr;
void *panicstk; 
void* clint_base = NULL;

extern void* plic_base;
extern void plic_init(void);

extern volatile uint64_t* mtime;
extern volatile uint64_t* mtimecmp;


#ifndef PLATFORMINIT_DETAIL
#define PLATFORMINIT_DETAIL 0
#endif

#if PLATFORMINIT_DETAIL
#define plinit_debug(format, ...) kprintf (format, ##__VA_ARGS__)
#else
#define plinit_debug(format, ...) 
#endif


void dtb_handler(char *nname,
		 char *pname,
		 unsigned char *val,
		 uint32_t len);
void apply_defaults(void);

int platforminit(void)
{
    plinit_debug("platformInit: enter\n");
    strlcpy(platform.family, "riscv64", PLT_STRMAX);
    strlcpy(platform.name, "riscv64 qemu virt", PLT_STRMAX);
   
#if PLATFORMINIT_DETAIL
    /* TODO: configure context switching code on basis of supported extensions,
       so far we do not support switching anything but base ISA registers */
    plinit_debug("platformInit: RISCV extensions supported: "); 
    int misa = read_csr(misa);
    int j;
    // extension names are ASCII letters correspondings to bit offsets
    for (j = 0; j < 26; j++) {
	if (misa & (1 << j)) {
	    plinit_debug("%c", 'A' + j);
	}
    }
    plinit_debug("\n");
#endif

    plinit_debug("dtb is at %0lx\n", dtb_addr);
    /* configure platfortm parameters from DeviceTree */
    parse_dtb(dtb_addr, dtb_handler, NULL);
    /* apply defaults for those parameters that were not set */
    apply_defaults();

    /* set timer registers offsets on core interrupter offset */
    mtime = (uint64_t*)(clint_base + 0xbff8);
    mtimecmp = (uint64_t*)(clint_base + 0x4000);
    // not used on RISC-V
    platform.serial_low = 0;   
    platform.serial_high = 0;

    /* initialize external and timer interrupts handling */
    plic_init();
    plinit_debug("platformInit: finished\n");
    return OK;
}

void mem_dtb_handler(char *nname,
		 char *pname,
		 unsigned char *val,
		 uint32_t len) {
    if ((!platform.maxaddr) &&
	(!strncmp(nname,"memory@",7)) &&
	(!strcmp(pname,"reg")) &&
	(len == 16)) {
	uint *dataptr = (uint32_t*)val;
	plinit_debug("mem_dtb_handler: setting memory size from dtb property %s/%s\n", nname, pname);
	platform.minaddr = (void*) dtb_read_uint64(dataptr);
	platform.maxaddr = (void*) platform.minaddr + dtb_read_uint64(dataptr + 2);
	plinit_debug("mem_dtb_handler: minaddr=0x%lX maxaddr=0x%lX\n", platform.minaddr, platform.maxaddr);
    }
}

void freq_dtb_handler(char *nname,
		      char *pname,
		      unsigned char *val,
		      uint32_t len) {
    if ((!platform.clkfreq) &&
	(!strcmp(nname,"cpus")) &&
	(!strcmp(pname,"timebase-frequency")) &&
	(len == 4)) {
	plinit_debug("freq_dtb_handler: clock frequency from dtb property %s/%s\n", nname, pname);
	platform.clkfreq = (uint64_t) be32_to_cpu(*((uint32_t*) val));
	plinit_debug("freq_dtb_handler: clock frequency set to %ld\n", platform.clkfreq);
    }
}

void clint_dtb_handler(char *nname,
		 char *pname,
		 unsigned char *val,
		 uint32_t len) {
    if ((!clint_base) &&
	(!strncmp(nname,"clint@",6)) &&
	(!strcmp(pname,"reg")) &&
	(len == 16)) {
	plinit_debug("clint_dtb_handler: setting CLINT offset from dtb property %s/%s\n", nname, pname);
	clint_base = (void*) dtb_read_uint64((uint32_t*)val);
	plinit_debug("clint_dtb_handler: CLINT is at  0x%lx\n", clint_base);
    }
}

void plic_dtb_handler(char *nname,
		 char *pname,
		 unsigned char *val,
		 uint32_t len) {
    if ((!plic_base) &&
	(!strncmp(nname,"interrupt-controller@",6)) &&
	(!strcmp(pname,"reg")) &&
	(len == 16)) {
	plinit_debug("plic_dtb_handler: setting PLIC offset from dtb property %s/%s\n", nname, pname);
	plic_base = (void*) dtb_read_uint64((uint32_t*)val);
	plinit_debug("plic_dtb_handler: PLIC is at  0x%lx\n", plic_base);
    }
}



void dtb_handler(char *nname,
		 char *pname,
		 unsigned char *val,
		 uint32_t len) {
    mem_dtb_handler(nname, pname, val, len);
    freq_dtb_handler(nname, pname, val, len);
    clint_dtb_handler(nname, pname, val, len);
    plic_dtb_handler(nname, pname, val, len);
}

void apply_defaults() {
    /* set default values in case devicetree parsing did not work (should not happen) */
    if (!platform.maxaddr) { 
	plinit_debug("platformInit: using fallback memory configuration\n"); 
	platform.minaddr = (void *) 0x80000000;
	platform.maxaddr = (void *) 0x87FFFFFF; 
    }                                  
    if (!platform.clkfreq) {
	platform.clkfreq = 10000000;
	plinit_debug("platformInit: using fallback clock frequency: %ld\n",platform.clkfreq); 
    }
    if (! clint_base) {
	clint_base = (void*) 0x2000000;
	plinit_debug("platformInit: using fallback CLINT base address is at 0x%lX\n", clint_base); 
    }
    if (! plic_base) {
	plic_base = (void*) 0xc000000;
	plinit_debug("platformInit: using fallback PLIC base address is at 0x%lX\n", plic_base); 
    }
}
    


