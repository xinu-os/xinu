/**
 * @file pic8259.h
 * 
 * PC-style i8259 programmable interrupt controller.
 *
 * $Id: pic8259.h 2157 2010-01-19 00:40:07Z brylow $
 */
/* Embedded Xinu, Copyright (C) 2009.  All rights reserved. */

#ifndef _PIC8259_H_
#define _PIC8259_H_

#ifdef PIC8259

#define PIC_MASTER     PIC8259
#define PIC_MASTER_IMR (PIC_MASTER + 1)
#define PIC_SLAVE      (PIC_MASTER + 0x80)
#define PIC_SLAVE_IMR  (PIC_SLAVE  + 1)
#define PIC_MASTER_ELCR (PIC_MASTER + 0x4B0)
#define PIC_SLAVE_ELCR (PIC_MASTER + 0x4B1)

#endif                          /* PIC8259 */

/* PIC function prototypes */

#endif                          /* _PIC8259_H_ */
