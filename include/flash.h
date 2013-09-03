/**
 * @file flash.h
 * This file provides various definitions and prototypes for the flash
 * driver and all its affiliates
 *
 */
/* Embedded XINU, Copyright (C) 2007.  All rights reserved. */

#include <kernel.h>
#include <device.h>
#include <semaphore.h>
#include <stdarg.h>

#ifndef _FLASH_H_
#define _FLASH_H_

/* Disk variables */
#define MAX_LIVE_BLOCKS 8   /**< number of blocks to keep in memory  */
#define FLASH_BLK_SIZE  512 /**< disk blocks are 512 bytes           */

#define FLASH_BLOCK_FREE  0 /**< block can be allocated              */
#define FLASH_BLOCK_CLEAN 1 /**< block has been read but not writted */
#define FLASH_BLOCK_DIRTY 2 /**< block has been changed              */

/* Important disk block sections */
#define CFE_MIN_BLOCK    0x0000 /**< Physical addr: 0x000000         */
#define CFE_MAX_BLOCK    0x01FF /**< Physical addr: 0x03ffff         */
#define KERNEL_MIN_BLOCK 0x0200 /**< Physical addr: 0x040000         */
#define KERNEL_MAX_BLOCK 0x03FF /**< Physical addr: 0x07ffff         */
#define NVRAM_MIN_BLOCK  0x1FC0 /**< Physical addr: 0x3F8000         */
#define NVRAM_MAX_BLOCK  0x1FFF /**< Physical addr: 0x3fffff         */

/* Flash control switch values */
#define FLASH_BLOCK_SIZE 0x0000 /**< get the size of disk blocks */
#define FLASH_N_BLOCKS   0x0001 /**< get number of disk blocks   */
#define FLASH_SYNC       0x0002 /**< force sync of disk device   */

/* Flash variables */
#define FLASH_WIDEMODE 1
#define MAX_REGIONS    8

/* TRX variables */
#define TRX_MAGIC   0x30524448

/* Structures */
struct trx_header
{
    uint magic;                 /**< "HDR0"                              */
    uint len;                   /**< Length of file including header     */
    uint crc;                   /**< 32-bit CRC from flag_vers to EOF    */
    uint flags_vers;             /**< 0-15 flags, 16-31 version           */
    uint offsets[3];            /**< Partition offsets from header start */
};

/* File structure */
struct file
{
    uint len;
    char *name;
    void *data;
};

struct flash_region
{
    ulong nblocks;              /**< number of blocks within region      */
    ulong block_size;           /**< size of block in region             */
    ulong region_start;         /**< beginning of region in memory       */
    ulong region_size;          /**< size of region in bytes             */
};

struct flash_block
{
    ulong start_pos;            /**< physical position data starts at    */
    ulong size;                 /**< size of this block                  */
    uchar state;                /**< state of this block                 */
    void *buffer;               /**< buffer for this block               */
};

struct flash
{
    device *device;             /**< flash entry in dev structure        */
    uchar commands;             /**< command set the interface uses      */
    ulong base;                 /**< base address for flash memory       */
    semaphore lock;             /**< lock for execution flash operations */
    ulong size;                 /**< size (in bytes) of flash memory     */
    uchar mode;                 /**< current mode of flash               */
    ushort nregions;            /**< number of regions on device         */
    struct flash_region regions[MAX_REGIONS];     /**< region info.      */

    ulong log_size;             /**< size of logical disk blocks         */
    ulong nlog_blocks;          /**< number of logical blocks on disk    */

    /** list of open blocks on flash device                              */
    struct flash_block erase_blocks[MAX_LIVE_BLOCKS];
    uchar curr_block;           /**< offset of the oldest block stored   */
};

extern struct flash_block bad_block;

/*
 * Common Flash Interface Query commands and macros
 */

/* put/get 8-bits commands for CFI */
#define CFI_PUT_8(addr,offset,cmd) \
	*((volatile unsigned char *)((addr)+((offset)<<FLASH_WIDEMODE)))=(cmd)
#define CFI_GET_8(addr,offset) \
	*((volatile unsigned char *)((addr)+((offset)<<FLASH_WIDEMODE)))

/* commands for data gathering */
#define CFI_QUERY_ADDR    0x55
#define CFI_QUERY_MODE    0x98

#define CFI_MANUFACTURER  0x00
#define CFI_DEVICE        0x01
#define CFI_SIGNATURE     0x10
#define CFI_QUERY_STRING  0x10
#define CFI_COMMAND_SET   0x13
#define CFI_DEV_SIZE      0x27
#define CFI_DEV_INTERFACE 0x28
#define CFI_MAX_BUFFER    0x2A
#define CFI_REGION_COUNT  0x2C
#define CFI_REGION_TAB    0x2D

/* Known command sets (from CFI publication 100) */
#define FLASH_INTEL_ECS   0x0001
#define FLASH_AMD_SCS     0x0002
#define FLASH_INTEL_SCS   0x0003
#define FLASH_AMD_ECS     0x0004
#define FLASH_MTSU_SCS    0x0100
#define FLASH_MTSU_ECS    0x0101

/*
 * Intel Flash specifc commands and macros
 */

/* put/get 8-bits commands for Intel */
#define INTEL_PUT_8(addr,byte) \
	*((volatile unsigned char *)((unsigned int)(addr)))=(byte)
#define INTEL_GET_8(addr) \
	*((volatile unsigned char *)((unsigned int)(addr)))

/* put/get 8-bits commands for AMD */
#define AMD_PUT_8(addr,byte) \
    *((volatile unsigned char *)((unsigned int)(addr)))=(byte)
#define AMD_GET_8(addr) \
    *((volatile unsigned char *)((unsigned int)(addr)))

/* put/get 16-bits commands for Intel */
#define INTEL_PUT_16(addr,word) \
	*((volatile unsigned short *)((unsigned int)(addr)))=(word)
#define INTEL_GET_16(addr) \
	*((volatile unsigned short *)((unsigned int)(addr)))

/* put/get 16-bits commands for AMD */
#define AMD_PUT_16(addr,word) \
    *((volatile unsigned short *)((unsigned int)(addr)))=(word)
#define AMD_GET_16(addr) \
        *((volatile unsigned short *)((unsigned int)(addr)))


/* Intel command codes                                               */
#define INTEL_READ     0xFF   /**< read array (normal read ops)      */
#define INTEL_PROGRAM  0x40   /**< prepare to write to flash         */
#define INTEL_ERASE    0x20   /**< prepare to erase a block          */
#define INTEL_CONFIRM  0xD0   /**< confirm erase command             */
#define INTEL_RESUME   0xD0   /**< write/erase resume (post-suspend) */
#define INTEL_UNLOCK   0xD0   /**< soft-unlock a block               */
#define INTEL_SUSPEND  0xB0   /**< program/erase suspend             */
#define INTEL_READ_SR  0x70   /**< read status register              */
#define INTEL_CLR_SR   0x50   /**< clear status register             */
#define INTEL_READ_ID  0x90   /**< read identifier                   */
#define INTEL_BLOCKMOD 0x60   /**< prepare block for lock operation  */
#define INTEL_LOCK     0x01   /**< soft-lock a block                 */
#define INTEL_LOCKDOWN 0x2F   /**< put a hardware lock on a block    */

/* AMD Command Codes                                                 */
#define AMD_UNLOCK_CYCLE1   0xAA    /**< write to 0x555              */
#define AMD_UNLOCK_CYCLE2   0x55    /**< write to 0x2AA              */
#define AMD_ERASE_CYCLE3    0x80    /**< write to 0x555              */
#define AMD_ERASE_CYCLE6    0x30    /**< write to erase address      */
#define AMD_PROGRAM_CYCLE3  0xA0    /**< write to 0x555              */
#define AMD_RESET           0xF0    /**< write to 0xXXX              */
#define AMD_READ            0xFF    /**< read command                */
#define AMD_BLOCKMOD        0x60    /**< prepare block for lock op   */
#define AMD_LOCK            0x01    /**< soft-lock a block           */
#define AMD_CLR_SR          0x50    /**< clear status register       */

/* AMD                                                               */
#define AMD_REG_CYCLE1  0x555      /**< Register for unlock cycle1   */
#define AMD_REG_CYCLE2  0x2AA      /**< Register for unlock cycle2   */
#define AMD_SR_WSMS     0x001      /**< Write State Machine Status   */

/* Intel status register offsets                                     */
#define INTEL_SR_WSMS  7      /**< Write State Machine Status        */
#define INTEL_SR_ESS   6      /**< Erase-Suspend Status              */
#define INTEL_SR_ES    5      /**< Erase Status                      */
#define INTEL_SR_PS    4      /**< Program Status                    */
#define INTEL_SR_PSS   2      /**< Program Suspend Status            */
#define INTEL_SR_BLS   1      /**< Block Lock Status                 */

/* Intel identifier register offsets                                 */
#define INTEL_ID_MFG   0x00   /**< Intel = 0x0089                    */
#define INTEL_ID_DEV   0x02   /**< Device memory-map                 */
#define INTEL_ID_BLS   0x04   /**< bit: 0 = soft-lock, 1 = hard-lock */

/* Intel block lock constants                                        */
#define INTEL_S_LOCK   0x01   /**< software lock                     */
#define INTEL_H_LOCK   0x02   /**< lock-down (hardward lock)         */

/* AMD block lock constants                                        */
#define AMD_S_LOCK   0x01   /**< software lock                     */
#define AMD_H_LOCK   0x02   /**< lock-down (hardward lock)         */

/* flashControl constants                                            */
#define FLASH_BLOCK    0x00 /**< block is a flash_block struct     */
#define FLASH_LOGBLOCK 0x01 /**< block is a logical block number   */

/* physicalLock commands                                             */
#define FLASH_STATUS   0x0000 /**< return status register (8-bits)   */
#define FLASH_MODE     0x0001 /**< change mode device is in          */
#define FLASH_LOCKED   0x0010 /**< return id register (16-bits)      */
#define FLASH_LOCK     0x0011 /**< put a software lock on a block    */
#define FLASH_UNLOCK   0x0012 /**< remove a software lock on a block */
#define FLASH_LOCKDOWN 0x0013 /**< put hardware lock on a block      */
#define FLASH_ERASE_PREP 0x0014 /**< prepare hardware to be erased   */
#define FLASH_RESET    0x0015

extern struct flash flashtab[];

/* Prototypes for flash function calls */
devcall flashInit(device *);
devcall flashOpen(device *, va_list);
devcall flashRead(device *, uchar *, ulong);
devcall flashWrite(device *, uchar *, ulong);
devcall flashSeek(device *, ulong);
devcall flashControl(device *, ushort, long, long);
devcall flashClose(device *);

/* Prototypes for the Logical Flash Interface */
devcall logicalRead(struct flash *, uchar *, ulong);
devcall logicalWrite(struct flash *, uchar *, ulong);
struct flash_block logicalMap(struct flash *, ulong);

/* Prototypes for Physical Flash functions */
devcall physicalRead(struct flash *, struct flash_block *);
devcall physicalWrite(struct flash *, struct flash_block *);
devcall physicalErase(struct flash *, struct flash_block *);
devcall physicalControl(struct flash *, struct flash_block *, uchar,
                        ulong);
devcall physicalIntelRead(struct flash *, struct flash_block *);
devcall physicalIntelWrite(struct flash *, struct flash_block *);
devcall physicalIntelErase(struct flash *, struct flash_block *);
devcall physicalIntelControl(struct flash *, struct flash_block *, uchar,
                             ulong);
devcall physicalAMDRead(struct flash *, struct flash_block *);

/* Prototypes for changing OS in flash memory */
bool validateTrx(struct file *);

/* File function prototypes */
syscall printFileStats(struct file *, char *, char *, int, int);
syscall freeFile(struct file *);

#endif                          /* _FLASH_H_ */
