/**
 * @file panic.c
 */
/* Embedded Xinu, Copyright (C) 2018.  All rights reserved. */
//#include <platform.h>
#include <kernel.h>
#include <riscv.h>

extern __attribute__((noreturn)) void halt(void);

/* panic message when caught exception that we cannot handle */
void __attribute__((noreturn)) handle_exception(ulong sp)
{
    /* TODO: print all registers
       decode cause codes */
    kprintf("**** handle_exception  *****\n");
    ulong cause = read_csr(mcause);
    ulong tval = read_csr(mtval);
    ulong mstatus = read_csr(mstatus);
    kprintf("sp = 0x%lx mstatus=0x%lx\n", sp, mstatus);
    kprintf("mcause = 0x%lx mtval=0x%lx\n", cause, tval);
    kprintf("at addr mepc=%lx\n",read_csr(mepc));
    halt();
}
